/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef DECODER_H_
#define DECODER_H_

#include <AcousticModel.h>
#include <Sample.h>
#include <SearchGraphLanguageModel.h>
#include <HMM.h>
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class SGNode {
 public:
  SGNode();
  SGNode(const uint32_t state_id, const float lprob, const float hmmlprob,
         const float lmlprob, const uint32_t hyp);

  uint32_t getStateId() { return state_id; }
  float getLProb() { return lprob; }
  float getHMMLProb() { return hmmlprob; }
  float getLMLProb() { return lmlprob; }
  uint32_t getHyp() { return hyp; }
  void setLProb(float lprob) { this->lprob = lprob; }
  void setHMMLProb(float hmmlprob) { this->hmmlprob = hmmlprob; }
  void setLMLProb(float lmlprob) { this->lmlprob = lmlprob; }
  void setHyp(uint32_t hyp) { this->hyp = hyp; }
  bool operator==(const SGNode& p) const { return state_id == p.state_id; }
  void showState();

 private:
  uint32_t state_id;
  float lprob;
  float hmmlprob;
  float lmlprob;
  uint32_t hyp;
};

class WordHyp {
 public:
  WordHyp(const int prev, const std::string& word);

  void showWordHyp();

 private:
  int prev;
  std::string word;
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
   * @brief Decode a sample providing the transcription.
   *
   * @param[in] sample
   * @return float
   */
  float decode(Sample sample);

  /**
   * @brief Expand a list of SGNodes.
   *
   * @param[in] searchgraph_nodes
   */
  void expand_search_graph_nodes(
      std::vector<std::unique_ptr<SGNode>>& searchgraph_nodes);

  /**
   * @brief Inserts a SGNode either in the null_nodes1 list (nodes that do not
   * contain symbols or words) or the nodes1 list (nodes that contain symbols or
   * words).
   *
   * @param[in] node
   */
  void insert_search_graph_node(std::unique_ptr<SGNode>& node);

  /**
   * @brief TO DO
   *
   * @param hmmNode
   */
  void insert_hmm_node(std::unique_ptr<HMMNode>& hmmNode);

  /**
   * @brief Add a SGNode to null_nodes0. These nodes will be expanded during the
   * iteration and this list will end up being empty.
   *
   * @param[in] node
   * @return int
   */
  int addNodeToSearchGraphNullNodes0(std::unique_ptr<SGNode>& node);

  /**
   * @brief Add a SGNode to null_nodes1 list. Temporal list to store null nodes
   * (nodes that do not contain symbols or words), that will be expanded again
   * during the same iteration.
   *
   * @param[in] node
   * @return int
   */
  int addNodeToSearchGraphNullNodes1(std::unique_ptr<SGNode>& node);

  /**
   * @brief Add a SGNode to nodes1 list. This list contains SGNode that contain
   * a symbol or a word, they will be expanded during this iteration.
   *
   * @param[in] node
   * @return int
   */
  int addNodeToSearchGraphNodes1(std::unique_ptr<SGNode>& node);

  /**
   * @brief Updates LM beam
   *
   * @param[in] lprob
   */
  void updateLmBeam(float lprob);

  /**
   * @brief Get the Search Graph Null Nodes0 list
   *
   * @return std::vector<std::unique_ptr<SGNode>>&
   */
  std::vector<std::unique_ptr<SGNode>>& getSearchGraphNullNodes0() {
    return search_graph_null_nodes0;
  }

  /**
   * @brief Get the Search Graph Null Nodes1 list
   *
   * @return std::vector<std::unique_ptr<SGNode>>&
   */
  std::vector<std::unique_ptr<SGNode>>& getSearchGraphNullNodes1() {
    return search_graph_null_nodes1;
  }

  /**
   * @brief Get the Search Graph Nodes0 list
   *
   * @return std::vector<std::unique_ptr<SGNode>>&
   */
  std::vector<std::unique_ptr<SGNode>>& getSearchGraphNodes0() {
    return search_graph_nodes0;
  }
  /**
   * @brief Get the Search Graph Nodes1 list
   *
   * @return std::vector<std::unique_ptr<SGNode>>&
   */
  std::vector<std::unique_ptr<SGNode>>& getSearchGraphNodes1() {
    return search_graph_nodes1;
  }

  /**
   * @brief Get the Actives list
   *
   * @return std::vector<int>
   */
  std::vector<int> getActives() { return actives; }

  /**
   * @brief
   *
   * @param[in] nodes
   */
  void printSGNodes(const std::vector<std::unique_ptr<SGNode>>& nodes);
  /**
   * @brief TO DO
   *
   * @return true
   * @return false
   */
  bool nodes0IsNotEmpty();
  /**
   * @brief TO DO
   *
   * @return true
   * @return false
   */
  bool nodes1IsNotEmpty();
  /**
   * @brief TO DO
   *
   * @return true
   * @return false
   */
  bool nullNodes0IsNotEmpty();
  /**
   * @brief TO DO
   *
   * @return true
   * @return false
   */
  bool nullNodes1IsNotEmpty();
  /**
   * @brief TO DO
   *
   * @param t
   */
  void viterbiIterSG(const int t);

  /**
   * @brief
   *
   */

  void viterbiSg2HMM();
  /**
   * @brief
   *
   * @return true
   * @return false
   */
  bool getReadyNullNodes();
  /**
   * @brief
   *
   * @return true
   * @return false
   */
  bool getReadyNodes();

  /**
   * @brief
   *
   * @return std::vector<std::unique_ptr<HMMNode>>&
   */
  std::vector<std::unique_ptr<HMMNode>>& getHMMNodes0() { return hmm_nodes0; }
  /**
   * @brief
   *
   * @return std::vector<std::unique_ptr<HMMNode>>&
   */
  std::vector<std::unique_ptr<HMMNode>>& getHMMNodes1() { return hmm_nodes1; }

  /**
   * @brief Get the Min Prob From HMM Nodes object
   *
   * @return float
   */
  float getMinProbFromHMMNodes();

  /**
   * @brief
   *
   * @param nodeId
   * @return int
   */
  int getHMMPosIfActive(const HMMNodeId& nodeId);
  /**
   * @brief
   *
   * @param nodeId
   * @param position
   * @return int
   */
  int setHMMActive(HMMNodeId nodeId, int position);

 private:
  std::unique_ptr<SearchGraphLanguageModel> sgraph;
  std::unique_ptr<AcousticModel> amodel;
  std::unique_ptr<SGNode> searchgraph_null_nodes0;
  std::vector<int> actives;
  std::vector<std::unique_ptr<SGNode>> search_graph_null_nodes0;
  std::vector<std::unique_ptr<SGNode>> search_graph_null_nodes1;
  std::vector<std::unique_ptr<SGNode>> search_graph_nodes0;
  std::vector<std::unique_ptr<SGNode>> search_graph_nodes1;

  std::vector<std::unique_ptr<HMMNode>> hmm_nodes0;
  std::vector<std::unique_ptr<HMMNode>> hmm_nodes1;
  std::unordered_map<HMMNodeId, int, HMMNodeIdHasher> HMMActives;

  std::vector<WordHyp> hypothesis;
  float v_thr = -HUGE_VAL;
  float v_lm_max = 0.0;
  float v_lm_beam = 0.0;
  float v_lm_thr = 0.0;
  uint32_t nmaxstates = 100;
  bool final_iter = false;
  float GSF = 10;
  float WIP = 0;
};

#include "Decoder.inl"

#endif  // DECODER_H_
