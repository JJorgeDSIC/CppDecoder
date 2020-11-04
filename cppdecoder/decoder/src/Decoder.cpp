/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include <Decoder.h>

SGNode::SGNode() : state_id(0), lprob(0.0), hmmlprob(0.0), lmlprob(0), hyp(0) {}

SGNode::SGNode(const uint32_t state_id, const float lprob, const float hmmlprob,
               const float lmlprob, const uint32_t hyp)
    : state_id(state_id),
      lprob(lprob),
      hmmlprob(hmmlprob),
      lmlprob(lmlprob),
      hyp(hyp) {}

WordHyp::WordHyp(const int prev, const std::string& word)
    : prev(prev), word(word) {}

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

float Decoder::decode(Sample sample) {
  // Init structs
  // HMMNodeManager
  // viterbi_init with the first frame
  viterbiInit(sample);
  // RESET AM CACHE
  for (auto it = sample.cbegin(); it != sample.cend(); ++it) {
    std::cout << ' ' << (*it).getDim() << std::endl;
  }

  // for each frame, perform:
  //  viterbi iteration

  // Final iteration

  // Get and print the results

  return this->amodel->calc_logprob("a", 0, sample.getFrame(0).getFeatures());
}

float Decoder::compute_lprob(const Frame& frame, const std::string& sym,
                             const int q) {
  // TODO: USE CACHE HERE
  return this->amodel->calc_logprob(sym, q, frame.getFeatures());
}

void Decoder::viterbiIter(const Sample& sample, const int t,
                          const bool finalIter) {
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
      std::cout << "Pruned by old_thr: " << node->getLogProb() << " < "
                << old_thr << std::endl;
      continue;
    }

    // std::cout << "Node at position: " << i << std::endl;
    node->showHMMState();

    // Adjust node->p with old_max
    node->setLogprob(node->getLogProb() - old_max);

    node->showHMMState();

    // Get symbol
    const std::string& symbol = sgraph->getIdToSym(node->getId().sg_state);
    std::cout << symbol << ", id: " << node->getId().sg_state << std::endl;
    // According to prune before, compute emission and update states
    if (prune_before) {
      auxp =
          compute_lprob(sample.getFrame(t), symbol, node->getId().hmm_q_state);
      node->setLogprob(node->getLogProb() + auxp);
      node->setHMMLogProb(node->getHMMLogProb() + auxp);
      // std::cout << "auxp: " << auxp << std::endl;
      if (node->getLogProb() == -HUGE_VAL) {
        continue;
      }
    }
    // TODO
    // Stuff for state alignment
    // amodel->
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
      std::cout << "p0: " << p0 << ", p1: " << p1 << std::endl;
      // TODO: CREATE NEW NODES!!

      std::unique_ptr<HMMNode> new_node(new HMMNode(
          node->getId().sg_state, node->getId().hmm_q_state, node->getLogProb(),
          node->getHMMLogProb(), node->getLMLogProb(), 0, 0));

      if (p0 != -HUGE_VAL) {
        node->setLogprob(current_p + p0);
        node->setHMMLogProb(current_hmmp + p0);
        // std::cout << "Inserting HMM node: loop\n" << std::endl;
        insert_hmm_node(std::move(node));
        hmmNodesExpanded++;
      }

      new_node->setLogprob(current_p + p1);
      new_node->setHMMLogProb(current_hmmp + p1);
      // TODO: Provide the proper param for number of Q
      inLastQ = !((new_node->getId().hmm_q_state + 1) < transVector.size());
      if (!inLastQ) {
        // std::cout << "Inserting HMM node: next\n" << std::endl;
        hmmNodesExpanded++;
        new_node->setIdQ(new_node->getId().hmm_q_state + 1);
        if (!final_iter) insert_hmm_node(std::move(new_node));
      }

      // std::cout << "===========================" << std::endl;
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
      insert_search_graph_node(sgnode);
    }
  }

  hmm_minheap_nodes1->showHeapContent();

  std::cout << "HMM nodes expanded: " << hmmNodesExpanded << std::endl;
  std::cout << "SG nodes expanded: " << sgNodesExpanded << std::endl;

  if (nodes1IsNotEmpty()) {
    viterbiIterSG(t);
    viterbiSg2HMM(sample);
  }

  // TODO: Reset eprobs
  getReadyHMMNodes0();
}

void Decoder::viterbiInit(const Sample& sample) {
  std::unique_ptr<SGNode> sgnodeIni(
      new SGNode(sgraph->getStartState(), 0.0, 0.0, 0.0, 0));

  insert_search_graph_node(sgnodeIni);

  viterbiIterSG(0);

  viterbiSg2HMM(sample);

  getReadyHMMNodes0();
}

void Decoder::updateLmBeam(float lprob) {
  v_lm_max = lprob;
  v_lm_thr = lprob - v_lm_beam;
}

void Decoder::expand_search_graph_nodes(
    std::vector<std::unique_ptr<SGNode>>& searchgraph_nodes) {
  float max_prob = -HUGE_VAL;
  int max_hyp = -1;
  SGNode* max_node = nullptr;
  SGNode* prev_node = nullptr;

  assert(searchgraph_nodes.size() != 0);

  std::cout << "Size: " << searchgraph_nodes.size() << std::endl;

  while (searchgraph_nodes.size() != 0) {
    std::unique_ptr<SGNode> node = std::move(searchgraph_nodes.back());
    std::cout << "Expanding: ****** " << std::endl;
    node->showState();
    searchgraph_nodes.pop_back();
    std::cout << node->getStateId() << std::endl;
    if (final_iter && node->getStateId() == sgraph->getFinalState()) {
      if (node->getLProb() > max_prob) {
        max_prob = node->getLProb();
        max_node = node.get();
        max_hyp = node->getHyp();
      }
      // TODO Register final trans
    }

    float curr_lprob = node->getLProb();
    float curr_lmlprob = node->getLMLProb();

    // SearchGraphLanguageModelState sgstate =
    // sgraph->getSearchGraphState(node->getStateId());
    SearchGraphLanguageModelState sgstate =
        sgraph->getSearchGraphState(node->getStateId());

    // std::cout << sgstate.edge_begin << std::endl;
    // std::cout << sgstate.edge_end << std::endl;

    for (uint32_t i = sgstate.edge_begin; i < sgstate.edge_end; i++) {
      SearchGraphLanguageModelEdge sgedge = sgraph->getSearchGraphEdge(i);

      std::cout << "sgedge.dst:  " << sgedge.dst << ", sgedge.weight "
                << sgedge.weight << " sym: " << sgraph->getIdToSym(sgedge.dst)
                << " word: " << sgraph->getIdToWord(sgedge.dst) << std::endl;

      // TODO Final iter
      if (!final_iter) {
        if (sgedge.dst == sgraph->getFinalState()) {
          continue;
        }
      } else {
        const std::string& symbol = sgraph->getIdToSym(sgedge.dst);
        std::cout << symbol << std::endl;
        if (symbol == "-") {
          continue;
        }
      }

      float lprob = curr_lprob + sgedge.weight * GSF + WIP;

      std::cout << "Lprob: " << lprob << std::endl;
      std::cout << "GSF: " << GSF << std::endl;

      float lmlprob = curr_lmlprob + sgedge.weight;
      std::unique_ptr<SGNode> sgnode(
          new SGNode(sgedge.dst, lprob, 0.0, lmlprob, node->getHyp()));
      std::cout << "Inserting: " << std::endl;
      sgnode->showState();
      insert_search_graph_node(sgnode);
    }
  }
}

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

  for (const auto& node : search_graph_null_nodes0) {
    node->showState();
  }

  std::cout << "------------------------" << std::endl;

  for (const auto& node : search_graph_null_nodes1) {
    node->showState();
  }
  search_graph_null_nodes0.swap(search_graph_null_nodes1);

  std::cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=" << std::endl;
  for (const auto& node : search_graph_null_nodes0) {
    node->showState();
  }
  std::cout << "------------------------" << std::endl;

  for (const auto& node : search_graph_null_nodes1) {
    node->showState();
  }
}

bool Decoder::getReadyNodes() {
  for (const auto& node : search_graph_nodes1) {
    actives[node->getStateId()] = -1;
  }
  for (const auto& node : search_graph_nodes0) {
    node->showState();
  }
  std::cout << "------------------------" << std::endl;

  for (const auto& node : search_graph_nodes1) {
    node->showState();
  }
  search_graph_nodes0.swap(search_graph_nodes1);

  std::cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=" << std::endl;
  for (const auto& node : search_graph_nodes0) {
    node->showState();
  }
  std::cout << "------------------------" << std::endl;

  for (const auto& node : search_graph_nodes1) {
    node->showState();
  }
}

void Decoder::viterbiIterSG(const int t) {
  std::cout << "viterbiIterSG" << std::endl;

  // Clean null nodes: clean actives, copy null nodes1 to null nodes0
  getReadyNullNodes();
  // Clean nodes: clean actives, copy nodes1 to nodes0
  getReadyNodes();

  if (nodes0IsNotEmpty()) {
    expand_search_graph_nodes(getSearchGraphNodes0());
  }
  for (const auto& node : search_graph_null_nodes0) {
    node->showState();
  }
  if (nullNodes0IsNotEmpty()) {
    expand_search_graph_nodes(getSearchGraphNullNodes0());
  }
  while (nullNodes1IsNotEmpty()) {
    getReadyNullNodes();
    expand_search_graph_nodes(getSearchGraphNullNodes0());
  }
  getReadyNodes();
}

void Decoder::viterbiSg2HMM(const Sample& sample) {
  std::vector<std::unique_ptr<SGNode>>& nodes0 = getSearchGraphNodes0();

  std::cout << "-----" << nodes0.size() << std::endl;

  for (const auto& node : nodes0) {
    node->showState();

    // Create new node...
    std::unique_ptr<HMMNode> new_node(
        new HMMNode(node->getStateId(), 0, node->getLProb(),
                    node->getHMMLProb(), node->getLMLProb(), 0, 0));

    const std::string& symbol = sgraph->getIdToSym(new_node->getId().sg_state);
    const std::string& transType = amodel->getStateTransType(symbol);

    std::cout << "Reading ID: s= " << new_node->getId().sg_state
              << ", q= " << new_node->getId().hmm_q_state
              << ", symbol= " << symbol << ", Trans type: " << transType
              << std::endl;

    if (transType == "Trans") {
      new_node->setIdQ(0);
      new_node->setLogprob(node->getLProb());
      new_node->setHMMLogProb(node->getHMMLProb());
      new_node->showHMMState();
      insert_hmm_node(std::move(new_node));
    } else {
      // TODO: alternative transition representation...
      assert(false);
    }
  }
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

  // std::cout << "v_thr: " << v_thr << ",v_max: " << v_max
  //           << ", v_abeam: " << v_abeam << std::endl;

  if (prob < v_thr) {
    std::cout << "Pruned because of prob < v_thr: " << prob << " < " << v_thr
              << std::endl;
    return;
  }

  full = hmm_minheap_nodes1->getNodes().size() == nmaxstates;

  if (full && prob <= getMinProbFromHMMNodes()) {
    std::cout << "Pruned because of full && prob <= getMinProbFromHMMNodes()"
              << std::endl;
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
    hmm_minheap_nodes1->updateNodeAt(position, prob, auxp);
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

void Decoder::insert_search_graph_node(std::unique_ptr<SGNode>& node) {
  const std::string& symbol = sgraph->getIdToSym(node->getStateId());
  const std::string& word = sgraph->getIdToWord(node->getStateId());

  bool insertWord = word != "-" && word != ">";

  bool nullNode = symbol == "-";

  // if (nullNode) {
  //   // Inserts in sg_null_nodes1
  //   std::cout << "sg_null_nodes1" << std::endl;
  // } else {
  //   // Insert in sg_nodes1
  //   std::cout << "sg_nodes1" << std::endl;
  // }

  // Not visited yet
  if (actives[node->getStateId()] == -1) {
    if (node->getLProb() > v_lm_max) {
      updateLmBeam(node->getLProb());
    }

    std::cout << "Not visited" << std::endl;

    if (insertWord) {
      hypothesis.emplace_back(node->getHyp(), word);
      node->setHyp(hypothesis.size() - 1);
      node->setHMMLProb(0.0);
      node->setLMLProb(0.0);
    }
    int node_id = node->getStateId();

    if (nullNode) {
      std::cout << "nullNode" << std::endl;
      actives[node_id] = addNodeToSearchGraphNullNodes1(node);
    } else {
      actives[node_id] = addNodeToSearchGraphNodes1(node);
    }

  } else {
    // std::cout << "Visited" << std::endl;
    // std::cout << "Update state" << std::endl;
    int position = actives[node->getStateId()];
    // std::cout << "position: " << position << std::endl;
    SGNode* prevNode = nullNode ? search_graph_null_nodes1[position].get()
                                : search_graph_nodes1[position].get();

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