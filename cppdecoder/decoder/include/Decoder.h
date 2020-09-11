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
 public:
  SGNode();
  SGNode(const int state_id, const float lprob, const float hmmlprob,
         const float lmlprob, const int hyp);

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
  void showState();

 private:
  int state_id;
  float lprob;
  float hmmlprob;
  float lmlprob;
  int hyp;
};

class HMMNode {
 private:
  uint32_t s;
};

class WordHyp {
 public:
  WordHyp(const int prev, const std::string& word);

  void showWordHyp();

 private:
  int prev;
  std::string word;
};

class HMMNodeManager {
 public:
  explicit HMMNodeManager(const uint32_t max_size);

  bool isFull();

 private:
  uint32_t max_hyps;
  uint32_t max_size;
  uint32_t size;
};

class Decoder {
 public:
  /**
   * @brief Construct a new Decoder object
   *
   * @param[in] sgraph
   * @param[in] amodel
   */
  Decoder(std::unique_ptr<SearchGraphLanguageModel> sgraph,
          std::unique_ptr<AcousticModel> amodel);

  /**
   * @brief
   *
   * @param[in] sample
   * @return float
   */
  float decode(Sample sample);

  /**
   * @brief
   *
   * @param[in] searchgraph_null_nodes0
   */
  void expand_search_graph_nodes(
      std::vector<std::unique_ptr<SGNode>> searchgraph_null_nodes0);

  /**
   * @brief
   *
   * @param[in] searchgraph_null_nodes0
   */
  void expand_search_graph_nodes(std::vector<SGNode> searchgraph_null_nodes0);

  /**
   * @brief
   *
   * @param[in] node
   */
  void insert_search_graph_node(std::unique_ptr<SGNode> node);

  /**
   * @brief
   *
   * @param[in] node
   * @return int
   */
  int addNodeToSearchGraphNullNodes0(std::unique_ptr<SGNode> node);

  /**
   * @brief
   *
   * @param[in] node
   * @return int
   */
  int addNodeToSearchGraphNullNodes1(std::unique_ptr<SGNode> node);

  /**
   * @brief
   *
   * @param[in] node
   * @return int
   */
  int addNodeToSearchGraphNodes1(std::unique_ptr<SGNode> node);

  /**
   * @brief
   *
   * @param[in] lprob
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
   * @param[in] nodes
   */
  void printSGNode(const std::vector<std::unique_ptr<SGNode>>& nodes);

 private:
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
};

#include "Decoder.inl"

#endif  // MIXTUREACOUSTICMODEL_H_
