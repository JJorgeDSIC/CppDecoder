/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef HMM_H_
#define HMM_H_

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * This class represents the id of the HMM node, that is, the Search Graph State
 * id and the HMM state, this conforms the global id (sg id, hmm state).
 *
 */
class HMMNodeId {
 public:
  uint32_t sg_state;
  uint32_t hmm_q_state;
  HMMNodeId();
  HMMNodeId(const uint32_t sg_state, const uint32_t hmm_q_state)
      : sg_state(sg_state), hmm_q_state(hmm_q_state) {}
  bool operator==(const HMMNodeId& other) const {
    return (sg_state == other.sg_state && hmm_q_state == other.hmm_q_state);
  }
};

struct HMMNodeIdHasher {
  std::size_t operator()(const HMMNodeId& k) const {
    using std::hash;
    using std::size_t;

    return ((hash<uint32_t>()(k.sg_state) ^
             (hash<uint32_t>()(k.hmm_q_state) << 1)));
  }
};

/**
 * This class represents an HMM Node, with:
 *  -The node's id, that is the Search Graph State plus the HMM state, (SG state
 * id, HMM state) -Log probability (lprob) in this node, that accumulates the
 * log probability so far. -HMM log probability (hmmlp), log probability at HMM
 * state level. -Language Model log probability (lmlp), log probability at
 * Search Graph state level. -Trapos: position in the HMM min heap. -Hypothesis
 * index (h), index of the hypothesis in the hypothesis vector that is stored in
 * the Decoder class.
 */
class HMMNode {
 private:
  HMMNodeId id;
  float lprob;
  float hmmlp;
  float lmlp;
  uint32_t trapos;
  uint32_t hyp_index;

 public:
  /**
   * @brief Construct a new HMMNode object
   *
   */
  HMMNode();
  /**
   * @brief Construct a new HMMNode object
   *
   * @param sg_state
   * @param hmm_q_state
   */
  HMMNode(const uint32_t sg_state, const uint32_t hmm_q_state);
  /**
   * @brief Construct a new HMMNode object
   *
   * @param sg_state
   * @param hmm_q_state
   * @param lprob
   * @param hmmlp
   * @param lmlp
   * @param trapos
   * @param hyp_index
   */
  HMMNode(const uint32_t sg_state, const uint32_t hmm_q_state,
          const float lprob, const float hmmlp, const float lmlp,
          const uint32_t trapos, const uint32_t hyp_index);
  /**
   * @brief Get the HMMNodeId object (searchgraph state, hmm state)
   *
   * @return const HMMNodeId& Return the HMMNodeId object (searchgraph state,
   * hmm state)
   */
  const HMMNodeId& getId() const { return id; }
  /**
   * @brief Get the Log Prob of this node
   *
   * @return float Log probability of this node
   */
  float getLogProb() { return lprob; }
  /**
   * @brief Get the log probability at HMM state level
   *
   * @return float log probability at HMM state level
   */
  float getHMMLogProb() { return hmmlp; }
  /**
   * @brief Get the log probability at Language model level
   *
   * @return float log probability at Language model level
   */
  float getLMLogProb() { return lmlp; }
  /**
   * @brief Get the position in the HMM min nodes heap
   *
   * @return uint32_t Position in the HMM min nodes heap
   */
  uint32_t getTrapos() { return trapos; }
  /**
   * @brief Get the hypothesis index in the hypothesis structure in the decoder
   * class.
   *
   * @return uint32_t Position in the vector hypothesis of the hypothesis
   * associated with this node
   */
  uint32_t getH() { return hyp_index; }
  /**
   * @brief Set the HMM state for this node, tipically 0,1,2 for common HMM
   * models
   *
   * @param q HMM state (i.e: 0,1,2)
   */
  void setIdQ(uint32_t q) { id.hmm_q_state = q; }
  /**
   * @brief Set the log probability for this node
   *
   * @param lprob Log probability
   */
  void setLogprob(float lprob) { this->lprob = lprob; }
  /**
   * @brief Set log probability at HMM state level
   *
   * @param hmmlp log probability at HMM state level
   */
  void setHMMLogProb(float hmmlp) { this->hmmlp = hmmlp; }
  /**
   * @brief Set the log probability at Language model level
   *
   * @param lmlp log probability at Language model level
   */
  void setLMLogProb(float lmlp) { this->lmlp = lmlp; }
  /**
   * @brief Set the position in the HMM min heap nodes.
   *
   * @param trapos Position in the min heap
   */
  void setTrapos(uint32_t trapos) { this->trapos = trapos; }
  /**
   * @brief Set the hypothesis index
   *
   * @param hyp_index Hypothesis index in the hypothesis structure in the
   * decoder class
   */
  void setH(uint32_t hyp_index) { this->hyp_index = hyp_index; }

  /**
   * @brief Prints to the stdout the content of the node
   *
   */
  void showHMMState();
};

class HMMMinHeap {
 public:
  /**
   * @brief Construct a new HMMMinHeap object with a given capacity.
   *
   * @param capacity Initial capacity for the min heap.
   */
  explicit HMMMinHeap(uint32_t capacity) : capacity(capacity), size(0) {
    hmm_nodes.resize(capacity + 1);
  }
  /**
   * @brief Get the minimum log probability in the heap
   *
   * @return float Minimum log probability
   */
  float getMinLProb();
  /**
   * @brief Get the nodes' vector, it is stored as a heap (tree-like), and the
   * position 0 is not used.
   *
   * @return std::vector<std::unique_ptr<HMMNode>>& Vector with the nodes in the
   * heap, position 0 is not used
   */
  std::vector<std::unique_ptr<HMMNode>>& getNodes() { return hmm_nodes; }
  /**
   * @brief Extract the minimum log prob node from the heap and returns it
   *
   * @return std::unique_ptr<HMMNode> Node with the minimum log probability
   */
  std::unique_ptr<HMMNode> extractMinLProbHMMNode();
  /**
   * @brief Inserts a node in the heap. If required, the capacity will be
   * expanded doubling the size of the heap. It returns the position in the
   * heap to allow direct modifications of nodes without searching.
   *
   * @param hmm_node The new node to be inserted
   * @return int Position in the heap.
   */
  int insert(std::unique_ptr<HMMNode> hmm_node);
  /**
   * @brief Removes the node with minimum log prob and inserts a new one.
   *
   * @param hmm_node The new node to be inserted
   * @return std::unique_ptr<HMMNode> Minimum log prob node that will be
   * extracted
   */
  std::unique_ptr<HMMNode> popAndInsert(std::unique_ptr<HMMNode> hmm_node);
  /**
   * @brief Updates the log probability of the node at the given position,
   * providing the log prob and the hmm log prob.
   *
   * @param position Position of the node in the heap
   * @param lprob New log prob
   * @param hmmlp New HMM state level log prob
   * @return int New position in the heap after updating.
   */
  int updateNodeAt(int position, float lprob, float hmmlp);
  /**
   * @brief Bubble up a node at the given position, returning its correct
   * position according to the heap structure. This method will exchange the
   * nodes internally during the process to accomodate the new node.
   *
   * @param hmm_node New node to be used to find the correct position according
   * to the heap structure
   * @param position  Current position of the new node
   * @return int Correct position according to the heap structure
   */
  int bubbleUp(const std::unique_ptr<HMMNode>& hmm_node, int position);
  /**
   * @brief Sinks the node at the provided position in the heap, according to
   * the heap structure, finding its correct position.
   *
   * @param position Position to start the sinking process
   * @return int Position after sinking the node at the given position
   */
  int sink(int position);
  /**
   * @brief Doubles the size of the heap and copies the nodes to the new
   * structure.
   *
   */
  void expandCapacity();

  /**
   * @brief Exchange actives nodes structure with the provided HMM min heap
   *
   * @param hmm_min_heap_other HMM min heap to exchange active nodes with
   */
  void exchangeActives(const std::unique_ptr<HMMMinHeap>& hmm_min_heap_other);
  /**
   * @brief Exchange nodes structure with the provided HMM min heap
   *
   * @param hmm_min_heap_other HMM min heap to exchange nodes with
   */
  void exchangeNodes(const std::unique_ptr<HMMMinHeap>& hmm_min_heap_other);
  /**
   * @brief Prints to the stdout the content of the heap in Breadth First order.
   *
   */
  void showHeapContent();
  /**
   * @brief Get the position of a node with the provided id, the tuple
   * (sg_state, hmm_q_state)
   *
   * @param sg_state Search Graph state identifier
   * @param hmm_q_state HMM state identifier, tipically 0,1,2 in common HMM
   * models
   * @return int Position in the heap of the node with the provided identifier
   */
  int getNodePositionById(const uint32_t sg_state, const uint32_t hmm_q_state);
  /**
   * @brief Get the node with the provided id, the tuple (sg_state, hmm_q_state)
   *
   * @param sg_state Search Graph state identifier
   * @param hmm_q_state HMM state identifier, tipically 0,1,2 in common HMM
   * models
   * @return std::unique_ptr<HMMNode>& Node in the heap of the node with the
   * provided identifier
   */
  std::unique_ptr<HMMNode>& getNodeById(const uint32_t sg_state,
                                        const uint32_t hmm_q_state);
  /**
   * @brief Get the node at the provided position in the heap.
   *
   * @param position Position of the node in the heap
   * @return std::unique_ptr<HMMNode>&  Node at the provided position
   */
  std::unique_ptr<HMMNode>& getNodeAtPosition(int position);
  /**
   * @brief Get the size of the heap, the number of active states in the
   * structure.
   *
   * @return int The number of active states in the heap structure
   */
  int getSize() { return size; }
  /**
   * @brief Set the number of active states in the structure
   *
   * @param size number of active states in the structure
   */
  void setSize(uint32_t size) { this->size = size; }
  /**
   * @brief Reset the active nodes structure
   *
   */
  void cleanActives();
  /**
   * This method resets the following structures:
   *  -The HMM nodes structure that contains the nodes in a heap structure.
   *  -The Active nodes structure that controls which and where are the nodes in
   * the heap -Sets the size to 0
   * @brief Reset the HMM min heap
   *
   */
  void reset();

 private:
  uint32_t capacity;
  uint32_t size;
  std::vector<std::unique_ptr<HMMNode>> hmm_nodes;
  std::unordered_map<HMMNodeId, int, HMMNodeIdHasher> HMMActives;
};

#endif  // HMM_H_
