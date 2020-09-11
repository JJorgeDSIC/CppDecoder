/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef DECODER_H_
#define DECODER_H_

#include <AcousticModel.h>
#include <Sample.h>
#include <SearchGraphLanguageModel.h>

#include <memory>
#include <string>
#include <vector>

class SGNode {
  int state_id = 0;
  float lprob = 0.0;
  float hmmlprob = 0.0;
  float lmlprob = 0;
  int hyp = 0;

 public:
  SGNode(int state_id, float lprob, float hmmlprob, float lmlprob, int hyp)
      : state_id(state_id),
        lprob(lprob),
        hmmlprob(hmmlprob),
        lmlprob(lmlprob),
        hyp(hyp) {}
  int getStateId() { return state_id; }
  float getLProb() { return lprob; }
  float getHMMLProb() { return hmmlprob; }
  float getLMLProb() { return lmlprob; }
  int getHyp() { return hyp; }
  void setLProb(float lprob) { this->lprob = lprob; }
  void setHMMLProb(float hmmlprob) { this->hmmlprob = hmmlprob; }
  void setLMLProb(float lmlprob) { this->lmlprob = lmlprob; }
  void setHyp(int hyp) { this->hyp = hyp; }
  bool operator==(const SGNode& p) const { return state_id == p.state_id; }
  void showState() {
    std::cout << state_id << " " << lprob << " " << std::endl;
  }
};

class HMMNode {
  size_t s;
};

class WordHyp {
  int prev;
  std::string word;

 public:
  WordHyp(int prev, std::string word) : prev(prev), word(word) {}
  void showWordHyp() {
    std::cout << "Prev: " << prev << " Word: " << word << std::endl;
  }
};

class HMMNodeManager {
  size_t max_hyps;
  size_t max_size;
  size_t size;

 public:
  explicit HMMNodeManager(size_t max_size)
      : max_hyps(max_size), max_size(max_size + 1), size(0) {}
  bool isFull();
};

class Decoder {
  std::unique_ptr<SearchGraphLanguageModel> sgraph;
  std::unique_ptr<AcousticModel> amodel;
  std::unique_ptr<SGNode> searchgraph_null_nodes0;
  std::vector<int> actives;
  std::vector<std::unique_ptr<SGNode>> search_graph_null_nodes0;
  std::vector<std::unique_ptr<SGNode>> search_graph_null_nodes1;
  std::vector<std::unique_ptr<SGNode>> search_graph_nodes1;
  std::vector<WordHyp> hypothesis;
  float v_lm_max = 0.0;
  float v_lm_beam = 0.0;
  float v_lm_thr = 0.0;
  bool final_iter = false;
  float GSF = 1;
  float WIP = 1;

 public:
  /**
   * @brief Construct a new Decoder object
   *
   * @param sgraph
   * @param amodel
   */
  Decoder(std::unique_ptr<SearchGraphLanguageModel> sgraph,
          std::unique_ptr<AcousticModel> amodel);
  /**
   * @brief
   *
   * @param sample
   * @return float
   */
  float decode(Sample sample);
  /**
   * @brief
   *
   * @param searchgraph_null_nodes0
   */
  void expand_search_graph_nodes(
      std::vector<std::unique_ptr<SGNode>> searchgraph_null_nodes0);
  /**
   * @brief
   *
   * @param searchgraph_null_nodes0
   */
  void expand_search_graph_nodes(std::vector<SGNode> searchgraph_null_nodes0);
  /**
   * @brief
   *
   * @param node
   */
  void insert_search_graph_node(std::unique_ptr<SGNode> node);
  /**
   * @brief
   *
   * @param node
   * @return int
   */
  int addNodeToSearchGraphNullNodes0(std::unique_ptr<SGNode> node);
  /**
   * @brief
   *
   * @param node
   * @return int
   */
  int addNodeToSearchGraphNullNodes1(std::unique_ptr<SGNode> node);
  /**
   * @brief
   *
   * @param node
   * @return int
   */
  int addNodeToSearchGraphNodes1(std::unique_ptr<SGNode> node);
  /**
   * @brief
   *
   * @param lprob
   */
  void updateLmBeam(float lprob);
  /**
   * @brief Get the Search Graph Null Nodes0 object
   *
   * @return std::vector<std::unique_ptr<SGNode>>&
   */
  std::vector<std::unique_ptr<SGNode>>& getSearchGraphNullNodes0() {
    return search_graph_null_nodes0;
  }
  /**
   * @brief Get the Search Graph Null Nodes1 object
   *
   * @return std::vector<std::unique_ptr<SGNode>>&
   */
  std::vector<std::unique_ptr<SGNode>>& getSearchGraphNullNodes1() {
    return search_graph_null_nodes1;
  }
  /**
   * @brief Get the Search Graph Nodes1 object
   *
   * @return std::vector<std::unique_ptr<SGNode>>&
   */
  std::vector<std::unique_ptr<SGNode>>& getSearchGraphNodes1() {
    return search_graph_nodes1;
  }
  /**
   * @brief Get the Actives object
   *
   * @return std::vector<int>
   */
  std::vector<int> getActives() { return actives; }
  /**
   * @brief
   *
   * @param nodes
   */
  void printSGNode(const std::vector<std::unique_ptr<SGNode>>& nodes) {
    std::cout << "printSGNode" << std::endl;
    for (size_t i = 0; i < nodes.size(); i++) {
      std::cout << nodes[i]->getStateId() << std::endl;
      std::cout << sgraph->getIdToSym(nodes[i]->getStateId()) << std::endl;
      std::cout << sgraph->getIdToWord(nodes[i]->getStateId()) << std::endl;
    }
  }
};

#endif  // MIXTUREACOUSTICMODEL_H_
