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
   * Add a Search Graph node that does not contain a symbol/word, that is, a
   * null node, in the search_graph_null_nodes0. These nodes will be expanded
   * during the decoding iteration and this structure will end up being empty.
   * @brief Add a SGNode to null_nodes0.
   *
   * @param[in] node
   * @return int
   */
  int addNodeToSearchGraphNullNodes0(std::unique_ptr<SGNode>& node);

  /**
   * Add a Search Graph node that do not contain a symbol/word, that is, a null
   * node, in the search_graph_null_nodes1. These nodes will be expanded during
   * the decoding iteration and this structure will end up being empty.
   * @brief Add a SGNode to null_nodes1.
   *
   * @param[in] node
   * @return int
   */
  int addNodeToSearchGraphNullNodes1(std::unique_ptr<SGNode>& node);

  /**
   * Add a Search Graph node to the search_graph_nodes1. This structure contains
   * SGNodes that contain a symbol or a word, coming from the expansion of
   * previous null nodes. This nodes will end up being transformed into HMM
   * nodes in a posterior step. This structure is empty at the beginning of the
   * iteration and it is empty at the end of each iteration.
   * @brief Add a SGNode to nodes1 list.
   *
   * @param[in] node
   * @return int
   */
  int addNodeToSearchGraphNodes1(std::unique_ptr<SGNode>& node);

  /**
   *
   * This method updates the Language Model threshold after finding the maximum
   * log probability so far, updating the language model max. log prob and then
   * adjusting the language model threshold with the max. log prob minus the
   * language model beam provided as param to the decoder.
   * @brief Updates LM threshold
   *
   * @param[in] lprob Log probability that is the maximum value so far
   */
  void updateLmThreshold(float lprob);

  /**
   * This method updates the HMM threshold after finding the maximum
   * log probability so far, updating the HMM maximum log prob and then
   * adjusting the HMM log prob threshold with the max. log prob minus the
   * HMM beam (v_abeam) provided as param to the decoder.
   * @brief Updates the HMM threshold
   *
   * @param[in] lprob Log probability that is the maximum value so far
   * @param[in] hyp_index Hypothesis index of the best hypothesis so far
   */
  void updateHMMThreshold(float lprob, uint32_t hyp_index);

  /**
   *
   * This method inserts an HMM node (HMMNode) into the HMM nodes1 structure, if
   * the node is not pruned. Related to the pruning steps:
   *  -If the log prob of this node is lower than the current threshold (v_thr),
   * the node is pruned directly (not inserted).
   *  -If the number of maximum HMM states has been reached (nmaxstates param)
   * and the log prob is lower than the lowest log prob from the already
   * inserted nodes, the node is pruned. If the node gets over these prunings,
   * the position in the nodes1 structure is retrieved, then:
   *  -If position is 0, this means that is a new node. This could happen if the
   * node comes from a new SGNode that is transformed into an HMM node, i.e: at
   * the beginning of a word. First the HMM threshold is updated if this the
   * best log prob seen so far, and then depending on the number of max states,
   * an insert or a pop-and-insert operation is performed. This passes the
   * ownership of this node to the hmm_minheam_nodes1 structure.
   *  -If position is different from 0, means that this is an active node (it
   * was added before). If the log probability of this new version of the node
   * is better than the one that is stored in the hmm_minheap_nodes1 structure,
   * the threshold is updated is required and the node is updated in the
   * structure as well.
   *
   * @brief Inserts, if it is not pruned, an HMM Node (HMMNode) into the HMM
   * nodes1 structure.
   *
   * @param[in] hmmNode
   */
  void insertHMMNode(std::unique_ptr<HMMNode> hmmNode);

  /**
   * @brief Get the Search Graph Null Nodes0 that contains the Search graph
   * nodes that are not a symbol/word node for the current iteration.
   *
   * @return std::vector<std::unique_ptr<SGNode>>&
   */
  std::vector<std::unique_ptr<SGNode>>& getSearchGraphNullNodes0() {
    return search_graph_null_nodes0;
  }

  /**
   * @brief Get the Search Graph Null Nodes1 that contains the Search graph
   * nodes that are not symbol/word node for the next iteration.
   *
   * @return std::vector<std::unique_ptr<SGNode>>&
   */
  std::vector<std::unique_ptr<SGNode>>& getSearchGraphNullNodes1() {
    return search_graph_null_nodes1;
  }

  /**
   * @brief Get the Search Graph Nodes0 that contains the Search graph
   * nodes that are symbol/word nodes for the current iteration.
   *
   * @return std::vector<std::unique_ptr<SGNode>>&
   */
  std::vector<std::unique_ptr<SGNode>>& getSearchGraphNodes0() {
    return search_graph_nodes0;
  }
  /**
   * @brief Get the Search Graph Nodes1 that contains the Search graph
   * nodes that are symbol/word nodes for the next iteration.
   *
   * @return std::vector<std::unique_ptr<SGNode>>&
   */
  std::vector<std::unique_ptr<SGNode>>& getSearchGraphNodes1() {
    return search_graph_nodes1;
  }

  /**
   * @brief Get the Search Graph node position in either the
   * search_graph_null_nodes1 or search_graph_nodes1 from the Search graph
   * active nodes structure, depending on the kind of node (null or symbol/word
   * node), -1 if this is a new node.
   *
   * @return position Position on the corresponding search graph node structure
   * or -1 if not active.
   */
  int getSearchGraphNodePosition(uint32_t state_id) {
    return actives[state_id];
  }
  /**
   * @brief Set the Search Graph Node Position in the Search graph active nodes
   * structure.
   *
   * @param node_id Node identifier
   * @param position New position
   */
  void setSearchGraphNodePosition(uint32_t node_id, int position) {
    actives[node_id] = position;
  }

  /**
   * @brief Checks if search_graph_nodes0 is NOT empty.
   *
   * @return true It is not empty
   * @return false Otherwise
   */

  bool nodes0IsNotEmpty() { return search_graph_nodes0.size() != 0; }

  /**
   * @brief Checks if search_graph_nodes1 is NOT empty.
   *
   * @return true It is not empty
   * @return false Otherwise
   */

  bool nodes1IsNotEmpty() { return search_graph_nodes1.size() != 0; }

  /**
   * @brief Checks if search_graph_null_nodes0 is NOT empty.
   *
   * @return true It is not empty
   * @return false Otherwise
   */
  bool nullNodes0IsNotEmpty() { return search_graph_null_nodes0.size() != 0; }
  /**
   * @brief Checks if search_graph_null_nodes1 is NOT empty.
   *
   * @return true It is not empty
   * @return false Otherwise
   */
  bool nullNodes1IsNotEmpty() { return search_graph_null_nodes1.size() != 0; }

  /**
   * This method performs an iteration of the Viterbi algorithm part where
   * Search Graph nodes will be expanded. It starts by resetting Search Graph
   * Null nodes 0 and Search Grap (non-null) nodes 0 structures, cleaning
   * actives and exchanging nodes0 with nodes1 from previous iteration.
   *  First it expands nodes 0, this is a special case that can happen during
   * the beginning of recognition. Second, null nodes 0 will be expanded. After
   * this, all SGNodes are either in null nodes 1 or nodes 1.
   *  Finally, it iterates until null nodes 1 is empty, first getting null nodes
   * 0 ready (moving nodes from null nodes 1 to null nodes 0), and then
   * expanding null nodes 0. The idea is that when this iterative process ends,
   * all SG nodes are non-null nodes that are in nodes 1. Before leaving this
   * method, nodes 1 is exchanged with nodes 0, so all the Search Graph nodes
   * are in nodes 0 for the next step.
   *
   * @brief Performs the expansion of the Search Graph nodes during the
   * Viterbi's algorithm iteration. When this is completed, all search graph
   * nodes (if any) are in nodes 1 (search_graph_nodes1).
   *
   * @param t Position of the current frame, from 0 (first frame) to sample's
   * length - 1 (last frame).
   */
  void viterbiIterSG(const int t);

  /**
   * @brief Reset the active nodes structure and exchange Search Graph null
   * nodes 0 with Search Graph null nodes 1, the latter will end up empty.
   */
  void getReadyNullNodes();
  /**
   * @brief Reset the active nodes structure and exchange Search Graph nodes 0
   * with Search Graph nodes 1, the latter will end up empty.
   */
  void getReadyNodes();

  /**
   * @brief Clear Search Graph Nodes 0
   *
   */
  void clearNodes0() { search_graph_nodes0.clear(); }

  /**
   *
   * This method converts the SGNodes from Nodes 0 into initial HMM nodes,
   * creating these new nodes, propagating and setting the parameters of the
   * node: Log prob, HMM log prob LM log prob, and the hypothesis' index. This
   * method will call insertHMMnode to try to insert the node in the HMM min
   * heap nodes 1, but the node could be pruned during that step.
   *  After this method Nodes 0 will be empty.
   *
   * @brief Iterates over Search Graph Nodes 0 to creating HMM nodes from the
   * SGNodes and tries to insert them in HMM min heap nodes 1.
   *
   * @param sample
   */
  void viterbiSg2HMM(const Sample& sample);

  /**
   * @brief Get a vector with the HMM nodes in nodes 0, for this iteration.
   *
   * @return std::vector<std::unique_ptr<HMMNode>>& Vector with HMM nodes for
   * this iteration
   */
  std::vector<std::unique_ptr<HMMNode>>& getHMMNodes0() {
    return hmm_minheap_nodes0->getNodes();
  }
  /**
   * @brief Get a vector with the HMM nodes in nodes 1, for the following
   * iteration.
   *
   * @return std::vector<std::unique_ptr<HMMNode>>& Vector with HMM nodes for
   * the following iteration
   */
  std::vector<std::unique_ptr<HMMNode>>& getHMMNodes1() {
    return hmm_minheap_nodes1->getNodes();
  }

  /**
   * @brief Get the Number Active HMM nodes in HMM min heap Nodes 0.
   *
   * @return int Number of active nodes in nodes 0
   */
  int getNumberActiveHMMNodes0() { return hmm_minheap_nodes0->getSize(); }

  /**
   * @brief Get the Number Active HMM nodes in HMM min heap Nodes 1.
   *
   * @return int Number of active nodes in nodes 1
   */
  int getNumberActiveHMMNodes1() { return hmm_minheap_nodes1->getSize(); }

  /**
   * @brief Get the minimum log prob From HMM min heap nodes 1.
   *
   * @return float
   */
  float getMinProbFromHMMNodes() { return hmm_minheap_nodes1->getMinLProb(); }

  /**
   * @brief Get ready HMM min heap nodes 0, exchanging it with HMM min heap 1,
   * the latter will be empty after this method.
   *
   */
  void getReadyHMMNodes0();

  /**
   * This method starts the Viterbi algorithm, as follows:
   *  -Add the initial Search Graph node to start the decoding.
   *  -Perform an iteration of the algorithm at Search Graph level, expanding
   * this initial node and filling the Search Graph nodes 0 structure. -From the
   * Search Graph nodes 0 the HMM nodes will be created, providing the initial
   * HMM nodes to continue with the decoding algorithm.
   *
   * @brief Performs the first iteration of the Viterbi algorithm using the
   * provided sample.
   *
   * @param sample Sample with the frames to be recognised.
   */
  void viterbiInit(const Sample& sample);

  /**
   *
   * This method performs an iteration of the Viterbi algorithm (not the first
   * one) to decode the provided sample, expanding HMM min heap nodes 0,
   * following, for each HMM node:
   *  -Getting the current node from the HMM node structure.
   *  -Checking if can be pruned if the log prob is lower than the threshold
   * from the previous iteration. -Current node's log prob is adjusted according
   * to the previous maximum log prob. -Compute the emission log prob according
   * to the current frame and the HMM node (id, state in the HMM model and
   * symbol), updating the node's log prob and HMM log prob.
   *  -Depending on the type of transition (currently only supported linear
   * transition models), it is performed the transition between HMM states,
   * considering the loop transition or the forward transition. Creates a new
   * node for each case and calls insertHMMNode with the updated attributes
   * according to:
   *    -log prob = log prob + emission score + p0 or log prob = log prob +
   * emission score + p1
   *  -If the node is in the last HMM state, a new Search
   * Graph node is created and then it calls insertSearchGraphNode with this new
   * node to try to insert it.
   *  -After expanding all the HMM nodes, new Search Graph nodes that could have
   * been created are in Search Graph Nodes 1. Then, viterbiIterSG is called to
   * perform a SG Node expansion, followed by viterbiSg2HMM call to create the
   * new HMM nodes in HMM min heap nodes 1, resulted from this SG node
   * expansion. -Finally, the acoustic model cache is restarted and it is
   * perfomed the exchange of HMM min heap nodes 1 with HMM min heap nodes 0 to
   * be ready for the next iteration.
   * @brief Performs an iteration of the Viterbi algorithm to decode the
   * provided sample.
   *
   * @param sample Sample with the frames to be recognised.
   */
  void viterbiIter(const Sample& sample, const int t, const bool finalIter);

  /**
   * @brief Resets the acoustic model log probs cache.
   *
   */
  void resetAMCache() { lprob_cache.clear(); }

  /**
   * @brief Sets the Language Model beam
   *
   * @param v_lm_beam Language Model beam
   */
  void setVLMBeam(float v_lm_beam) { this->v_lm_beam = v_lm_beam; }

  /**
   * @brief Sets the HMM beam
   *
   * @param v_abeam HMM beam
   */
  void setVBeam(float v_abeam) { this->v_abeam = v_abeam; }

  /**
   * @brief Computes the emission log prob or score with a given frame
   * (n-dimensional vector), a symbol or senone and the state index of the HMM
   * model (q)
   *
   * @param frame Frame to be used to compute the log prob score
   * @param sym Symbol or senone, to index the HMM model
   * @param q State of the HMM model.
   * @return float Log probability or log(p(x,HMM(sym,q)))
   */
  float compute_lprob(const Frame& frame, const std::string& sym, const int q);

  /**
   * @brief Get the vector of WordHyps where the partial hypotheses are stored.
   *
   * @return std::vector<WordHyp>& Vector of WordHyp that contains (prev_index,
   * word string).
   */
  std::vector<WordHyp>& getWordHyps() { return hypothesis; }

  /**
   * @brief Get the hypothesis with the maximum log prob.
   *
   * @return int Index in hypothesis vector of the best hypothesis.
   */
  int getMaxHyp() { return max_hyp; }

  /**
   * @brief Get the maximum log prob so far.
   *
   * @return float Maximum log probability.
   */
  float getMaxProb() { return max_prob; }

  /**
   * This method resets the decoder state, as follows:
   *  -Reset HMM threshold, HMM max, LM max, LM beam, LM threshold and max
   * hypothesis. -Clear Search Graph actives structure -Clear Search Graph null
   * nodes 0 and 1 -Reset Search Graph nodes 0 and 1 -Reset HMM min heap nodes 0
   * and 1 -Clear Acoustic model log prob cache (just in case) -Clear the
   * current Hypothesis vector.
   *
   * @brief Reset all the structures and variables to perform the decoding with
   * another sample.
   *
   */
  void resetDecoder();

  /**
   * @brief Get the string with the sequence of words that were recognized.
   *
   * @return std::string The sequence of words that were recognized
   */
  std::string getResult();

  /** Acoustic Model cache related Hashing functions **/
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
};

#include "Decoder.inl"

#endif  // DECODER_H_
