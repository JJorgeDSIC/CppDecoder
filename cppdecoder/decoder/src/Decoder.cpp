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
}

float Decoder::decode(Sample sample) {
  // Init structs
  // HMMNodeManager
  // viterbi_init with the first frame
  // for each frame, perform:
  //  viterbi iteration

  // Final iteration

  // Get and print the results

  return this->amodel->calc_logprob("a", 0, sample.getFrame(0).getFeatures());
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

void Decoder::viterbiSg2HMM() {
  std::vector<std::unique_ptr<SGNode>>& nodes0 = getSearchGraphNodes0();

  std::cout << "-----" << nodes0.size() << std::endl;

  for (const auto& node : nodes0) {
    node->showState();

    // Create new node...
    std::unique_ptr<HMMNode> new_node(
        new HMMNode(node->getStateId(), 0, node->getLProb(),
                    node->getHMMLProb(), node->getLMLProb(), 0, 0));
    new_node->showHMMState();

    std::cout << "Reading ID: s= " << new_node->getId().sg_state
              << ", q= " << new_node->getId().hmm_q_state << std::endl;
    // Trans type conditional

    //
  }
}

void Decoder::insert_hmm_node(std::unique_ptr<HMMNode>& hmmNode) {
  // TODO: Prune before options
  bool full;
  float prob = 0.0;  // hmmNode->getLogprob();
  float auxp = 0.0;

  if (prob < v_thr) return;

  if ((full = hmm_nodes1.size() == nmaxstates) &&
      prob <= getMinProbFromHMMNodes())
    return;

  // TODO
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
  // TODO
  return 0.0;
}

int Decoder::getHMMPosIfActive(const HMMNodeId& nodeId) {
  auto result = HMMActives.find(nodeId);
  if (result != HMMActives.end()) {
    return result->second;
  } else {
    return -1;
  }
}

int Decoder::setHMMActive(HMMNodeId nodeId, int position) {
  HMMActives[nodeId] = position;
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