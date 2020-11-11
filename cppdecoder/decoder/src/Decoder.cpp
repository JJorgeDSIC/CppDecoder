/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include <Decoder.h>

/**
 * @brief Search Graph Node methods' definition
 *
 */

SGNode::SGNode() : state_id(0), lprob(0.0), hmmlprob(0.0), lmlprob(0), hyp(0) {}

SGNode::SGNode(const uint32_t state_id, const float lprob, const float hmmlprob,
               const float lmlprob, const uint32_t hyp)
    : state_id(state_id),
      lprob(lprob),
      hmmlprob(hmmlprob),
      lmlprob(lmlprob),
      hyp(hyp) {}

/**
 * @brief Word Hypothesis methods' definition
 *
 */

WordHyp::WordHyp(const int prev, const std::string& word)
    : prev(prev), word(word) {}

/**
 * @brief Decoder methods' definition
 *
 */

Decoder::Decoder(std::unique_ptr<SearchGraphLanguageModel> sgraph,
                 std::unique_ptr<AcousticModel> amodel) {
  this->sgraph = std::move(sgraph);
  this->amodel = std::move(amodel);
  std::vector<int> activesTemp(this->sgraph->getNStates(),
                               -1);  // Review this...
  actives = std::move(activesTemp);

  std::unique_ptr<HMMMinHeap> hmm_minheap_temp0(new HMMMinHeap(nmaxstates));
  std::unique_ptr<HMMMinHeap> hmm_minheap_temp1(new HMMMinHeap(nmaxstates));

  hmm_minheap_nodes0 = std::move(hmm_minheap_temp0);
  hmm_minheap_nodes1 = std::move(hmm_minheap_temp1);
}

// TODO: searchgraph_nodes candidate to be const?
void Decoder::expandSearchGraphNodes(
    std::vector<std::unique_ptr<SGNode>>& searchgraph_nodes) {
  max_prob = -HUGE_VAL;
  SGNode* max_node = nullptr;
  SGNode* prev_node = nullptr;
  float local_wip = 0.0;

  assert(searchgraph_nodes.size() != 0);

  for (size_t i = 0; i < searchgraph_nodes.size(); i++) {
    std::unique_ptr<SGNode>& node = searchgraph_nodes[i];

    if (final_iter && node->getStateId() == sgraph->getFinalState()) {
      if (node->getLProb() > max_prob) {
        max_prob = node->getLProb();
        max_node = node.get();
        max_hyp = node->getHyp();
      }
      // TODO Register final trans
      // TODO Store hypothesis at symbol level in the future
    }

    float curr_lprob = node->getLProb();
    float curr_lmlprob = node->getLMLProb();
    std::string wordInNode = sgraph->getIdToWord(node->getStateId());
    local_wip = (wordInNode != "-" && wordInNode != ">") ? WIP : 0;

    SearchGraphLanguageModelState sgstate =
        sgraph->getSearchGraphState(node->getStateId());

    for (uint32_t i = sgstate.edge_begin; i < sgstate.edge_end; i++) {
      SearchGraphLanguageModelEdge sgedge = sgraph->getSearchGraphEdge(i);
      if (!final_iter) {
        if (sgedge.dst == sgraph->getFinalState()) {
          continue;
        }
      } else if (sgraph->getIdToSym(sgedge.dst) != "-") {
        continue;
      }

      float lprob = curr_lprob + sgedge.weight * GSF + local_wip;
      float lmlprob = curr_lmlprob + sgedge.weight;
      std::unique_ptr<SGNode> sgnode(new SGNode(
          sgedge.dst, lprob, node->getHMMLProb(), lmlprob, node->getHyp()));
      insertSearchGraphNode(sgnode);
    }
  }
}

void Decoder::insertSearchGraphNode(std::unique_ptr<SGNode>& node) {
  const std::string& symbol = sgraph->getIdToSym(node->getStateId());
  const std::string& word = sgraph->getIdToWord(node->getStateId());

  bool insertWord = word != "-" && word != ">";

  bool nullNode = symbol == "-";

  if (node->getLProb() < v_lm_thr) return;
  if (WIP <= 0 && node->getLProb() < v_thr) return;

  // Not visited yet
  if (actives[node->getStateId()] == -1) {
    if (node->getLProb() > v_lm_max) {
      updateLmBeam(node->getLProb());
    }

    if (insertWord) {
      // hypothesis.emplace_back(node->getHyp(), word);
      hypothesis.push_back(WordHyp(node->getHyp(), word));
      node->setHyp(hypothesis.size() - 1);
      node->setHMMLProb(0.0);
      node->setLMLProb(0.0);
    }
    int node_id = node->getStateId();

    if (nullNode) {
      actives[node_id] = addNodeToSearchGraphNullNodes1(node);
    } else {
      actives[node_id] = addNodeToSearchGraphNodes1(node);
    }

  } else {
    int position = actives[node->getStateId()];
    std::unique_ptr<SGNode>& prevNode = nullNode
                                            ? search_graph_null_nodes1[position]
                                            : search_graph_nodes1[position];

    if (node->getLProb() > prevNode->getLProb()) {
      if (node->getLProb() > v_lm_max) {
        updateLmBeam(node->getLProb());
      }
      prevNode->setLProb(node->getLProb());
      prevNode->setHMMLProb(node->getHMMLProb());
      prevNode->setLMLProb(node->getLMLProb());
      prevNode->setHyp(node->getHyp());
    }
  }
}

float Decoder::decode(Sample sample) {
  viterbiInit(sample);
  // TODO: Fix prune before case
  // TODO: Fix adaptative beam

  std::cout << "# of frames: " << sample.getNFrames() << std::endl;

  getWordHyps().emplace_back(-1, "");
  // for (size_t i = 0; i < sample.getNFrames(); i++) {
  setVLMBeam(HUGE_VAL);  // LM rescoring related...
  setVBeam(300);
  for (size_t i = 0; i < sample.getNFrames() - 1; i++) {
    // std::cout << sample.getFrame(i).getDim() << std::endl;
    // std::cout << "############### CURRENT FRAME: " << i << std::endl;

    viterbiIter(sample, i, false);
  }
  // std::cout << "############### CURRENT FRAME: " << sample.getNFrames() - 1
  //          << std::endl;

  viterbiIter(sample, sample.getNFrames() - 1, true);
  std::cout << "Result: " << std::endl;
  getResult();
}

float Decoder::compute_lprob(const Frame& frame, const std::string& sym,
                             const int q) {
  auto it = lprob_cache.find(cacheLProbID(sym, q));
  if (it != lprob_cache.end()) {
    return it->second;
  } else {
    float lprob = this->amodel->calc_logprob(sym, q, frame.getFeatures());
    lprob_cache[cacheLProbID(sym, q)] = lprob;
    return lprob;
  }
}

void Decoder::resetAMCache() { lprob_cache.clear(); }

void Decoder::viterbiIter(const Sample& sample, const int t,
                          const bool finalIter) {
  currentIteration = t;
  final_iter = finalIter;

  int hmmNodesExpanded = 0;
  int sgNodesExpanded = 0;
  float auxp = 0.0;
  float current_p = 0.0;
  float current_hmmp = 0.0;
  float current_lmp = 0.0;
  int current_hyp = 0;
  int nodeSGstate;

  // Expand hmm_nodes0
  std::vector<std::unique_ptr<HMMNode>>& nodes0 = this->getHMMNodes0();

  // set some variables for pruning
  float old_max = this->v_max;
  float old_thr = this->v_thr;
  this->v_max = -HUGE_VAL;
  this->v_thr = -HUGE_VAL;
  this->v_lm_max = -HUGE_VAL;
  this->v_lm_thr = -HUGE_VAL;
  bool inLastQ = false;
  float p0, p1;

  // TODO
  // TODO
  // TODO
  // Iterate over nodes in hmm_nodes0 (this is a vector representation of a
  // heap, 0 is empty)
  // For each node
  for (size_t i = this->getNumberActiveHMMNodes0(); i > 0; i--) {
    std::unique_ptr<HMMNode>& node = nodes0[i];
    // Extra pruning according to the old_thr
    if (!final_iter && node->getLogProb() < old_thr) {
      continue;
    }
    // Adjust node->p with old_max
    node->setLogprob(node->getLogProb() - old_max);

    // Get symbol
    const std::string& symbol = sgraph->getIdToSym(node->getId().sg_state);

    // According to prune before, compute emission and update states
    if (prune_before) {
      auxp =
          compute_lprob(sample.getFrame(t), symbol, node->getId().hmm_q_state);
      node->setLogprob(node->getLogProb() + auxp);
      node->setHMMLogProb(node->getHMMLogProb() + auxp);
      if (node->getLogProb() == -HUGE_VAL) {
        continue;
      }
    }
    // TODO
    // Stuff for state alignment
    // amodel->...
    // Perform transitions

    const std::string& transType = amodel->getStateTransType(symbol);
    if (transType == "Trans") {
      std::vector<float> transVector = amodel->getStateTrans(symbol);
      nodeSGstate = node->getId().sg_state;
      current_p = node->getLogProb();
      current_hmmp = node->getHMMLogProb();
      current_lmp = node->getLMLogProb();
      current_hyp = node->getH();

      // TODO: This could be done when AM is loaded...
      p1 = transVector[node->getId().hmm_q_state];
      p0 = log(1 - exp(p1));

      // TODO: CREATE NEW NODES!!
      std::unique_ptr<HMMNode> new_node(new HMMNode(
          node->getId().sg_state, node->getId().hmm_q_state, node->getLogProb(),
          node->getHMMLogProb(), node->getLMLogProb(), 0, node->getH()));

      if (p0 != -HUGE_VAL) {
        node->setLogprob(current_p + p0);
        node->setHMMLogProb(current_hmmp + p0);
        insert_hmm_node(std::move(node));
        hmmNodesExpanded++;
      }

      new_node->setLogprob(current_p + p1);
      new_node->setHMMLogProb(current_hmmp + p1);

      // TODO: Provide the proper param for number of Q
      inLastQ = !((new_node->getId().hmm_q_state + 1) < transVector.size());
      if (!inLastQ) {
        hmmNodesExpanded++;
        new_node->setIdQ(new_node->getId().hmm_q_state + 1);
        if (!final_iter) insert_hmm_node(std::move(new_node));
      }

    } else {
      // TODO: alternative transition representation...
      assert(false);
    }

    // If final, do some stuff
    if (inLastQ) {
      std::unique_ptr<SGNode> sgnode(new SGNode(nodeSGstate, current_p + p1,
                                                current_hmmp + p1, current_lmp,
                                                current_hyp));
      sgNodesExpanded++;
      insertSearchGraphNode(sgnode);
    }
  }

  if (nodes1IsNotEmpty()) {
    viterbiIterSG(t);
    viterbiSg2HMM(sample);
  }

  // TODO: More efficient way to do this?
  resetAMCache();
  getReadyHMMNodes0();
}

void Decoder::viterbiInit(const Sample& sample) {
  std::unique_ptr<SGNode> sgnodeIni(
      new SGNode(sgraph->getStartState(), 0.0, 0.0, 0.0, 0));

  insertSearchGraphNode(sgnodeIni);

  viterbiIterSG(0);

  viterbiSg2HMM(sample);

  getReadyHMMNodes0();
}

void Decoder::updateLmBeam(float lprob) {
  v_lm_max = lprob;
  v_lm_thr = lprob - v_lm_beam;
}

void Decoder::clearNodes0() { search_graph_nodes0.clear(); }

bool Decoder::nodes0IsNotEmpty() { return search_graph_nodes0.size() != 0; }

bool Decoder::nodes1IsNotEmpty() { return search_graph_nodes1.size() != 0; }

bool Decoder::nullNodes0IsNotEmpty() {
  return search_graph_null_nodes0.size() != 0;
}

bool Decoder::nullNodes1IsNotEmpty() {
  return search_graph_null_nodes1.size() != 0;
}

bool Decoder::getReadyNullNodes() {
  for (const auto& node : search_graph_null_nodes1) {
    actives[node->getStateId()] = -1;
  }
  search_graph_null_nodes0.swap(search_graph_null_nodes1);
  search_graph_null_nodes1.clear();
}

bool Decoder::getReadyNodes() {
  for (const auto& node : search_graph_nodes1) {
    actives[node->getStateId()] = -1;
  }
  search_graph_nodes0.swap(search_graph_nodes1);
  search_graph_nodes1.clear();
}

void Decoder::viterbiIterSG(const int t) {
  // std::cout << "viterbiIterSG" << std::endl;

  // Clean null nodes: clean actives, copy null nodes1 to null nodes0
  getReadyNullNodes();
  // Clean nodes: clean actives, copy nodes1 to nodes0
  getReadyNodes();

  if (nodes0IsNotEmpty()) {
    expandSearchGraphNodes(getSearchGraphNodes0());
  }

  if (nullNodes0IsNotEmpty()) {
    expandSearchGraphNodes(getSearchGraphNullNodes0());
  }
  while (nullNodes1IsNotEmpty()) {
    getReadyNullNodes();
    expandSearchGraphNodes(getSearchGraphNullNodes0());
  }
  getReadyNodes();
}

void Decoder::viterbiSg2HMM(const Sample& sample) {
  std::vector<std::unique_ptr<SGNode>>& nodes0 = getSearchGraphNodes0();

  for (const auto& node : nodes0) {
    // Create new node...
    std::unique_ptr<HMMNode> new_node(new HMMNode(
        node->getStateId(), 0, node->getLProb(), node->getHMMLProb(),
        node->getLMLProb(), 0, node->getHyp()));

    const std::string& symbol = sgraph->getIdToSym(new_node->getId().sg_state);
    const std::string& transType = amodel->getStateTransType(symbol);

    if (transType == "Trans") {
      new_node->setIdQ(0);
      new_node->setLogprob(node->getLProb());
      new_node->setHMMLogProb(node->getHMMLProb());
      insert_hmm_node(std::move(new_node));
    } else {
      // TODO: alternative transition representation...
      assert(false);
    }
  }

  // Clear SG nodes
  // TODO: Think about preallocating the memory and manage these things
  clearNodes0();
}

std::vector<std::unique_ptr<HMMNode>>& Decoder::getHMMNodes0() {
  return hmm_minheap_nodes0->getNodes();
}

std::vector<std::unique_ptr<HMMNode>>& Decoder::getHMMNodes1() {
  return hmm_minheap_nodes1->getNodes();
}

int Decoder::getNumberActiveHMMNodes0() {
  return hmm_minheap_nodes0->getSize();
}

int Decoder::getNumberActiveHMMNodes1() {
  return hmm_minheap_nodes1->getSize();
}

void Decoder::getReadyHMMNodes0() {
  // TODO: Copy hmm_minheap_nodes1 into nodes0

  hmm_minheap_nodes0->getNodes().swap(hmm_minheap_nodes1->getNodes());
  hmm_minheap_nodes0->setSize(hmm_minheap_nodes1->getSize());
  hmm_minheap_nodes0->getActives().swap(hmm_minheap_nodes1->getActives());
  hmm_minheap_nodes1->setSize(0);
  hmm_minheap_nodes1->cleanActives();
}

void Decoder::insert_hmm_node(std::unique_ptr<HMMNode> hmmNode) {
  // TODO: Prune before options
  bool full;
  float prob = hmmNode->getLogProb();
  float auxp = 0.0;
  float tmp_p, tmp_hmmp;
  std::unique_ptr<HMMNode> aux;

  if (prob < v_thr) {
    return;
  }

  full = hmm_minheap_nodes1->getSize() == nmaxstates;

  if (full && prob <= getMinProbFromHMMNodes()) {
    return;
  }

  // TODO
  int position = hmm_minheap_nodes1->getNodePositionById(
      hmmNode->getId().sg_state, hmmNode->getId().hmm_q_state);

  if (position == 0) { /* New node */
    if (prob > v_max) {
      v_max = prob;
      v_thr = prob - v_abeam;

      v_maxh = hmmNode->getH();
    }

    tmp_p = hmmNode->getLogProb();
    tmp_hmmp = hmmNode->getHMMLogProb();
    hmmNode->setLogprob(prob);
    hmmNode->setHMMLogProb(hmmNode->getHMMLogProb() + auxp);
    if (full) {
      aux = hmm_minheap_nodes1->popAndInsert(std::move(hmmNode));
    } else {
      std::unique_ptr<HMMNode>& auxNode = hmmNode;
      int posis = hmm_minheap_nodes1->insert(std::move(hmmNode));
    }
    // node->p = tmp_p;
    // node->hmmp = tmp_hmmp;
  } else if (prob >
             hmm_minheap_nodes1->getNodeAtPosition(position)->getLogProb()) {
    // Revisited node
    if (prob > v_max) {
      v_max = prob;
      v_thr = prob - v_abeam;
      v_maxh = hmmNode->getH();
    }
    hmm_minheap_nodes1->updateNodeAt(position, prob, hmmNode->getHMMLogProb());
  }
}

int Decoder::addNodeToSearchGraphNullNodes0(std::unique_ptr<SGNode>& node) {
  search_graph_null_nodes0.push_back(std::move(node));
  return search_graph_null_nodes0.size() - 1;
}

int Decoder::addNodeToSearchGraphNullNodes1(std::unique_ptr<SGNode>& node) {
  search_graph_null_nodes1.push_back(std::move(node));
  return search_graph_null_nodes1.size() - 1;
}

int Decoder::addNodeToSearchGraphNodes1(std::unique_ptr<SGNode>& node) {
  search_graph_nodes1.push_back(std::move(node));
  return search_graph_nodes1.size() - 1;
}

float Decoder::getMinProbFromHMMNodes() {
  return hmm_minheap_nodes1->getMinLProb();
}

std::vector<WordHyp>& Decoder::getWordHyps() { return hypothesis; }

std::string Decoder::getResult() {
  // std::cout << "max prob: " << getMaxProb() << std::endl;
  // std::cout << "max hyps: " << getMaxHyp() << std::endl;

  std::vector<WordHyp> hyps = getWordHyps();

  uint32_t prev = getMaxHyp();

  std::vector<std::string> res;

  while (prev != 0) {
    res.push_back(hyps[prev].getWord());
    prev = hyps[prev].getPrev();
  }

  std::string output = "";

  for (int i = res.size() - 1; i > -1; i--) {
    // std::cout << res[i] << " ";
    output.append(res[i]);
    output.append(" ");
  }

  // std::cout << std::endl;
  return std::move(output);
}

void Decoder::resetDecoder() {
  v_thr = -HUGE_VAL;
  v_max = -HUGE_VAL;
  v_lm_max = -HUGE_VAL;
  v_lm_beam = HUGE_VAL;
  v_lm_thr = -HUGE_VAL;
  v_maxh = 0;
  nmaxstates = 100;
  final_iter = false;
  GSF = 10;
  WIP = 0;
  beam = 300;
  v_abeam = HUGE_VAL;
  max_hyp = -1;
  max_prob = -HUGE_VAL;
  currentIteration = 0;
  prune_before = true;

  actives.clear();
  search_graph_null_nodes0.clear();
  search_graph_null_nodes1.clear();
  search_graph_nodes0.clear();
  search_graph_nodes1.clear();

  hmm_minheap_nodes0.reset();
  hmm_minheap_nodes1.reset();

  lprob_cache.clear();
  hypothesis.clear();

  std::vector<int> activesTemp(this->sgraph->getNStates(),
                               -1);  // Review this...
  actives = std::move(activesTemp);

  std::unique_ptr<HMMMinHeap> hmm_minheap_temp0(new HMMMinHeap(nmaxstates));
  std::unique_ptr<HMMMinHeap> hmm_minheap_temp1(new HMMMinHeap(nmaxstates));

  hmm_minheap_nodes0 = std::move(hmm_minheap_temp0);
  hmm_minheap_nodes1 = std::move(hmm_minheap_temp1);
}