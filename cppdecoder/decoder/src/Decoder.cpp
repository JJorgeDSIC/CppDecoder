#include <Decoder.h>

Decoder::Decoder(std::unique_ptr<SearchGraphLanguageModel>& sgraph,
                 std::unique_ptr<AcousticModel>& amodel) {
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
    std::vector<SGNode>& searchgraph_null_nodes0) {
  // TODO
  std::cout << "TODO" << std::endl;
}

int Decoder::addNodeToSearchGraphNullNodes1(std::unique_ptr<SGNode>& node) {
  search_graph_null_nodes1.push_back(std::move(node));
  return search_graph_null_nodes1.size() - 1;
}

int Decoder::addNodeToSearchGraphNodes1(std::unique_ptr<SGNode>& node) {
  search_graph_nodes1.push_back(std::move(node));
  return search_graph_nodes1.size() - 1;
}

void Decoder::insert_search_graph_node(std::unique_ptr<SGNode>& node) {
  const std::string& symbol = sgraph->getIdToSym(node->getStateId());
  const std::string& word = sgraph->getIdToWord(node->getStateId());

  bool insertWord = word != "-" && word != ">";

  bool nullNode = symbol == "-";

  if (nullNode) {
    // Inserts in sg_null_nodes1
    std::cout << "sg_null_nodes1" << std::endl;
  } else {
    // Insert in sg_nodes1
    std::cout << "sg_nodes1" << std::endl;
  }

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
      actives[node_id] = addNodeToSearchGraphNullNodes1(node);
    } else {
      actives[node_id] = addNodeToSearchGraphNodes1(node);
    }

  } else {
    std::cout << "Visited" << std::endl;
    std::cout << "Update state" << std::endl;
    int position = actives[node->getStateId()];
    std::cout << "position: " << position << std::endl;
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