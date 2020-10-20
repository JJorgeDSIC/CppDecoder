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

class HMMNode {
 private:
  HMMNodeId id;
  float lprob;
  float hmmp;
  float lmp;
  uint32_t trapos;
  uint32_t h;

 public:
  HMMNode();
  HMMNode(const uint32_t sg_state, const uint32_t hmm_q_state);
  HMMNode(const uint32_t sg_state, const uint32_t hmm_q_state,
          const float lprob, const float hmmp, const float lmp,
          const uint32_t trapos, const uint32_t h);

  const HMMNodeId& getId() const { return id; }
  float getLogProb() { return lprob; }
  float getHMMLogProb() { return hmmp; }
  float getLMLogProb() { return lmp; }
  uint32_t getTrapos() { return trapos; }
  uint32_t getH() { return h; }

  void setLogprob(float lprob) { this->lprob = lprob; }
  void setHMMLogProb(float hmmp) { this->hmmp = hmmp; }
  void setLMLogProb(float lmp) { this->lmp = lmp; }
  void setTrapos(uint32_t trapos) { this->trapos = trapos; }
  void setH(uint32_t h) { this->h = h; }
  void showHMMState();
};

class HMMMinHeap {
 public:
  explicit HMMMinHeap(uint32_t capacity) : capacity(capacity), size(0) {
    hmm_nodes.resize(capacity + 1);
  }
  float getMinLProb();
  std::unique_ptr<HMMNode> extractMinLProbHMMNode();
  int insert(std::unique_ptr<HMMNode> hmm_node);
  std::unique_ptr<HMMNode> popAndInsert(std::unique_ptr<HMMNode> hmm_node);
  int update(const uint32_t sg_state, const uint32_t hmm_q_state, float lprob);
  int bubbleUp(const std::unique_ptr<HMMNode>& hmm_node, int position);
  int sink(int position);
  void expandCapacity();
  void showHeapContent();

  int getNodePositionById(const uint32_t sg_state, const uint32_t hmm_q_state);
  std::unique_ptr<HMMNode>& getNodeAtPosition(int position);

 private:
  uint32_t capacity;
  uint32_t size;
  std::vector<std::unique_ptr<HMMNode>> hmm_nodes;
  std::unordered_map<HMMNodeId, int, HMMNodeIdHasher> HMMActives;
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

#endif  // HMM_H_
