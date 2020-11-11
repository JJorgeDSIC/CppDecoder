/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef DECODER_H_
#define DECODER_H_

#include <AcousticModel.h>
#include <HMM.h>
#include <Sample.h>
#include <SearchGraphLanguageModel.h>

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * @brief This class provides the Search Graph nodes (SGNode) attributes and
 * functions, this could be seen as nodes in a graph at language model level.
 * It contains the unique identifier, the current accumulated log probability,
 * also at HMM and Language Model level, and the index of the hypothesis
 * associated with this state.
 *
 */
class SGNode {
 public:
  SGNode();
  SGNode(const uint32_t state_id, const float lprob, const float hmmlprob,
         const float lmlprob, const uint32_t hyp);

  /**
   * @brief Get the Id of this Search Graph Node
   *
   * @return uint32_t Unique identifier
   */
  uint32_t getStateId() { return state_id; }
  /**
   * @brief Get the log probability of the Search Graph Node
   *
   * @return float Log probability
   */
  float getLProb() { return lprob; }
  /**
   * @brief Get the log probability at HMM state level
   *
   * @return float Log probability at HMM level
   */
  float getHMMLProb() { return hmmlprob; }
  /**
   * @brief Get the log probability at language model state level
   *
   * @return float Log probability at Language model level
   */
  float getLMLProb() { return lmlprob; }
  /**
   * @brief Get the hypothesis index for this Search Graph node, this indexes
   * the hypothesis vector in the decoder
   *
   * @return uint32_t Index of the associated hypothesis
   */
  uint32_t getHyp() { return hyp; }

  /**
   * @brief Set Log probability for this Search Graph Node
   *
   * @param[in] lprob Log probability
   */
  void setLProb(float lprob) { this->lprob = lprob; }
  /**
   * @brief Set Log probability at HMM level for this Search Graph Node
   *
   * @param[in] lprob Log probability
   */
  void setHMMLProb(float hmmlprob) { this->hmmlprob = hmmlprob; }
  /**
   * @brief Set Log probability at Language model for this Search Graph Node
   *
   * @param[in] lprob Log probability
   */
  void setLMLProb(float lmlprob) { this->lmlprob = lmlprob; }
  /**
   * @brief Set the index of the hypothesis for this Search Graph Node
   *
   * @param[in] hyp
   */
  void setHyp(uint32_t hyp) { this->hyp = hyp; }

  /**
   * @brief Equals operator for Search Graph node, comparing the id
   *
   * @param[in] p Search Graph Node to be compared
   * @return true If they are equal
   * @return false otherwise
   */
  bool operator==(const SGNode& p) const { return state_id == p.state_id; }
  void showState();

 private:
  uint32_t state_id;
  float lprob;
  float hmmlprob;
  float lmlprob;
  uint32_t hyp;
};

/**
 * @brief This class provides the partial hypothesis at word level, with the
 * current word and the position of the previous hypothesis.
 *
 */

class WordHyp {
 public:
  /**
   * @brief Construct a new Word Hyp object with the index of the previous
   * hypothesis and the current word.
   *
   * @param prev Index to the previous hypothesis
   * @param word Word included in this hypothesis
   */
  WordHyp(const int prev, const std::string& word);

  /**
   * @brief Get the word (string) of this hypothesis
   *
   * @return std::string& Word on this hypothesis
   */
  std::string& getWord() { return word; }
  /**
   * @brief Get the previous index where the previous hypothesis is stored in
   * the internal decoder structure.
   *
   * @return int Position of the previous index
   */
  int getPrev() { return prev; }
  /**
   * @brief Show the current hypothesis, the index of the previos hypothesis and
   * the word on this hypothesis.
   *
   */
  void showWordHyp();

 private:
  int prev;
  std::string word;
};

class Decoder {
 public:
  /**
   *
   * This method moves the Search Grap and the acoustic model to the decoder, it
   * owns this models after this call. It initializes Search Graph active
   * hypothesis structure to its empty state, and it initializes also the HMM
   * min heaps with the number of max states allowed in the nmaxstates param.
   * There are two of this structure as it is required one for the current
   * iteration (hmm_minheap_nodes0) and the second one to store the ones for the
   * next iteration (hmm_minheap_nodes1). These structures will be exchanged at
   * the end of each iteration.
   * @brief Construct a new Decoder object with the Search Graph Language Model
   * and the Acoustic Model.
   *
   * @param[in] sgraph Search Graph Language Model, a graph-like structure with
   * nodes with LM related information (symbols, words) and weighted edges.
   * @param[in] amodel  Acoustic Model, an statistical model that provides the
   * transitions between HMM nodes and the emission log probabilities.
   */
  Decoder(std::unique_ptr<SearchGraphLanguageModel> sgraph,
          std::unique_ptr<AcousticModel> amodel);

  /**
   * @brief Decode a sample providing the transcription.
   *
   * @param[in] sample Sample that contains the feature vectors
   * @return float Log probability of this sample
   */
  float decode(Sample sample);

  /**
   * This method takes a non-empty vector of Search Graph nodes (SGNode) and
   * expand it. The process of this expansion involves iterating on each SGNode
   * in the vector once and:
   * -If this is the final iteration, that is, the last frame of the sample, and
   * the current node is connected to the node labeled as final state, the max
   * log probability is updated storing the max hypothesis and the current
   * SGNode.
   * -Expand edges of the current node, considering that if this is not the last
   * iteration (the last frame) to not include the incoming node that is the
   * final state in the Search Graph. Additionally, if this is the final
   * iteration and the incoming node is a null node (nodes without symbol or
   * word, labeled with '-') will not be indluced as well.
   * -For each incoming node (destiny node) a new SGNode will be created with
   * the log probability and the language model log probability updated
   * according to:
   *  - log_prob = current_log_prob + edge_weight * Grammar_Scale_Factor (GSF) +
   * Word_Insertion_Penalty (WIP)
   *  - LM log_prob = current_LM_log_prob + edge_weight
   * -Finally, this node is provided to the insertSearchGraphNode to,
   * potentially, be inserted in the internal decoder structure (check
   * insertSearchGraphNode for more details).
   * -The input vector is not modified in this method, being read-only.
   *
   * @brief Expand a list of SGNodes.
   *
   * @param[in] searchgraph_nodes
   */
  void expandSearchGraphNodes(
      std::vector<std::unique_ptr<SGNode>>& searchgraph_nodes);

  /**
   *
   * This method inserts a Search Graph Node (SGNode), if not pruned, in either
   * in search_graph_null_nodes1 (nodes that do not contain symbol or word) or
   * in search_graph_nodes1 (nodes that contain symbol or word). Considering the
   * following pruning steps, node will be pruned if:
   *  -If the node's log probability is lower than the language model threshold.
   *  -If WIP is negative or zero and the log probability of this node is lower
   * than the beam threshold.
   *
   * The method checks if this node is active (it was reached during this
   * iteration), if node is not active, so it is a new node:
   *  -If the node's log prob is higher than the LM max log prob (log prob at
   * SGNode level), this value is updated, and LM threshold is updated with
   * node's log prob and adjusted with the LM beam (lm_beam).
   *  -If this is a word node (it contains a word string), this will be included
   * in the hypothesis' vector. This sets log prob and HMM log prob to 0.0,
   * including the hypothesis' position in the node.
   *  -Depending on the kind of node (null node or symbol/word node), it will be
   * inserted in search_graph_null_nodes1 or search_graph_nodes1.
   *
   * In the event of this node being an already visited node (active node),
   * then:
   *  -Get the position of this node according to the active nodes
   * structure, and getting the previous inserted node from the corresponding
   * search graph nodes structure: search_graph_null_nodes1 or
   * search_graph_nodes1.
   *  -If this node has better log prob than the old one,
   * first the language model threshold (v_lm_thr) will be updated as in the non
   * active situation, and then the node's attributes will be updated: log prob,
   * hmm log prob, LM log prob and the hypothesis index.
   * @brief Inserts a SGNode either in the null_nodes1 list (nodes that do not
   * contain symbols or words) or the nodes1 list (nodes that contain symbols or
   * words), if it overcomes pruning thresholds.
   *
   * @param[in] node
   */
  void insertSearchGraphNode(std::unique_ptr<SGNode>& node);

  /**
   * @brief TO DO
   *
   * @param hmmNode
   */
  void insert_hmm_node(std::unique_ptr<HMMNode> hmmNode);

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

  void viterbiSg2HMM(const Sample& sample);
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
  std::vector<std::unique_ptr<HMMNode>>& getHMMNodes0();
  /**
   * @brief
   *
   * @return std::vector<std::unique_ptr<HMMNode>>&
   */
  std::vector<std::unique_ptr<HMMNode>>& getHMMNodes1();

  /**
   * @brief Get the Min Prob From HMM Nodes object
   *
   * @return float
   */
  float getMinProbFromHMMNodes();

  /**
   * @brief Get the Number Active H M M Nodes object
   *
   * @return int
   */
  int getNumberActiveHMMNodes();

  /**
   * @brief Get the Ready H M M Nodes0 object
   *
   */
  void getReadyHMMNodes0();

  /**
   * @brief Get the Number Active H M M Nodes0 object
   *
   * @return int
   */
  int getNumberActiveHMMNodes0();
  /**
   * @brief Get the Number Active H M M Nodes1 object
   *
   * @return int
   */
  int getNumberActiveHMMNodes1();

  /**
   * @brief
   *
   * @param sample
   */
  void viterbiInit(const Sample& sample);

  /**
   * @brief
   *
   * @param sample
   */
  void viterbiIter(const Sample& sample, const int t, const bool finalIter);

  /**
   * @brief
   *
   * @param v_lm_beam
   */
  void setVLMBeam(float v_lm_beam) { this->v_lm_beam = v_lm_beam; }
  /**
   * @brief
   *
   * @param v_lm_beam
   */
  void setVBeam(float v_abeam) { this->v_abeam = v_abeam; }

  /**
   * @brief
   *
   * @param frame
   * @param sym
   * @param q
   * @return float
   */
  float compute_lprob(const Frame& frame, const std::string& sym, const int q);

  /**
   * @brief
   *
   */
  void resetAMCache();

  /**
   * @brief
   *
   */
  void clearNodes0();

  /**
   * @brief TODO
   *
   * @return std::vector<WordHyp>&
   */
  std::vector<WordHyp>& getWordHyps();

  /**
   * @brief TODO
   *
   * @return int
   */
  int getMaxHyp() { return max_hyp; }

  /**
   * @brief TODO
   *
   * @return float
   */
  float getMaxProb() { return max_prob; }

  /**
   * @brief TODO
   *
   */
  void resetDecoder();

  std::string getResult();

  class cacheLProbID {
   public:
    std::string sym;
    uint32_t q;
    cacheLProbID();
    cacheLProbID(const std::string& sym, uint32_t q) : sym(sym), q(q) {}
    bool operator==(const cacheLProbID& other) const {
      return (sym == other.sym && q == other.q);
    }
  };

  struct cacheLProbIDHasher {
    std::size_t operator()(const cacheLProbID& k) const {
      using std::hash;
      using std::size_t;

      return ((hash<std::string>()(k.sym) ^ (hash<uint32_t>()(k.q) << 1)));
    }
  };

 private:
  std::unique_ptr<SearchGraphLanguageModel> sgraph;
  std::unique_ptr<AcousticModel> amodel;
  std::vector<int> actives;
  std::vector<std::unique_ptr<SGNode>> search_graph_null_nodes0;
  std::vector<std::unique_ptr<SGNode>> search_graph_null_nodes1;
  std::vector<std::unique_ptr<SGNode>> search_graph_nodes0;
  std::vector<std::unique_ptr<SGNode>> search_graph_nodes1;
  std::unique_ptr<HMMMinHeap> hmm_minheap_nodes0;
  std::unique_ptr<HMMMinHeap> hmm_minheap_nodes1;
  std::unordered_map<cacheLProbID, float, cacheLProbIDHasher> lprob_cache;

  std::vector<WordHyp> hypothesis;
  float v_thr = -HUGE_VAL;
  float v_max = -HUGE_VAL;
  float v_lm_max = -HUGE_VAL;
  float v_lm_beam = HUGE_VAL;
  float v_lm_thr = -HUGE_VAL;
  int v_maxh = 0;
  uint32_t nmaxstates = 100;
  bool final_iter = false;
  float GSF = 10;
  float WIP = 0;
  float beam = 300;
  float v_abeam = HUGE_VAL;
  int max_hyp = -1;
  float max_prob = -HUGE_VAL;
  int currentIteration = 0;
  bool prune_before = true;
};

#include "Decoder.inl"

#endif  // DECODER_H_
