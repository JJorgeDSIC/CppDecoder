/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include <HMM.h>

HMMNodeId::HMMNodeId() {
  this->sg_state = 0;
  this->hmm_q_state = 0;
}

HMMNode::HMMNode() : lprob(0.0), hmmp(0.0), lmp(0.0), trapos(0), h(0) {
  this->id = HMMNodeId(0, 0);
}

HMMNode::HMMNode(const uint32_t sg_state, const uint32_t hmm_q_state)
    : lprob(0.0), hmmp(0.0), lmp(0.0), trapos(0), h(0) {
  this->id = HMMNodeId(sg_state, hmm_q_state);
}

HMMNode::HMMNode(const uint32_t sg_state, const uint32_t hmm_q_state,
                 const float lprob, const float hmmp, const float lmp,
                 const uint32_t trapos, const uint32_t h)
    : lprob(lprob), hmmp(hmmp), lmp(lmp), trapos(trapos), h(h) {
  this->id = HMMNodeId(sg_state, hmm_q_state);
}

void HMMNode::showHMMState() {
  std::cout << "ID_S: " << id.sg_state << ", ID_Q: " << id.hmm_q_state
            << ", LProb: " << lprob << ", HMMProb: " << hmmp
            << ", LMProb: " << lmp << ", Trapos: " << trapos << ", h: " << h
            << std::endl;
}

float HMMMinHeap::getMinLProb() {
  assert(size > 0);
  return hmm_nodes[1]->getLogProb();
}

std::unique_ptr<HMMNode> HMMMinHeap::extractMinLProbHMMNode() {
  hmm_nodes[1].swap(hmm_nodes[size--]);
  std::unique_ptr<HMMNode> minNode = std::move(hmm_nodes[size + 1]);
  sink(1);
  return std::move(minNode);
}

int HMMMinHeap::sink(int position) {
  int currentPos = position;
  std::unique_ptr<HMMNode> nodeToSink = std::move(hmm_nodes[currentPos]);
  int son = currentPos * 2;
  bool isNotHeap = true;
  while (son <= size && isNotHeap) {
    if (son < size &&
        hmm_nodes[son + 1]->getLogProb() < hmm_nodes[son]->getLogProb())
      son++;
    if (hmm_nodes[son]->getLogProb() < nodeToSink->getLogProb()) {
      hmm_nodes[currentPos].swap(hmm_nodes[son]);
      currentPos = son;
      son = currentPos * 2;
    } else {
      isNotHeap = false;
    }
  }
  hmm_nodes[currentPos] = std::move(nodeToSink);
}
int HMMMinHeap::bubbleUp(std::unique_ptr<HMMNode>& hmm_node, int position) {
  while (position > 1 &&
         hmm_node->getLogProb() < hmm_nodes[position / 2]->getLogProb()) {
    hmm_nodes[position / 2].swap(hmm_nodes[position]);
    position = position / 2;
  }
  return position;
}

int HMMMinHeap::insert(std::unique_ptr<HMMNode> hmm_node) {
  int position = size + 1;

  assert(size <= capacity);

  if (size == 0) {  // Empty heap
    position = 1;
    size++;
    hmm_nodes[position] = std::move(hmm_node);
  } else {
    position = bubbleUp(hmm_node, position);
    hmm_nodes[position] = std::move(hmm_node);
    size++;
  }
  return position;
}

int HMMMinHeap::pop_add(std::unique_ptr<HMMNode> hmm_node) { return -1; }

int HMMMinHeap::update(std::unique_ptr<HMMNode> hmm_node) { return -1; }

void HMMMinHeap::showHeapContent() {
  for (size_t i = 1; i < size + 1; i++) {
    hmm_nodes[i]->showHMMState();
  }
}

HMMNodeManager::HMMNodeManager(const uint32_t max_size)
    : max_hyps(max_size), max_size(max_size + 1), size(0) {}
