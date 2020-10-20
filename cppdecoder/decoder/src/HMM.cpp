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
  HMMActives[hmm_nodes[1]->getId()] = -1;
  HMMActives[hmm_nodes[size]->getId()] = 1;

  hmm_nodes[1].swap(hmm_nodes[size--]);

  std::unique_ptr<HMMNode> minNode = std::move(hmm_nodes[size + 1]);
  if (size > 1) sink(1);
  return std::move(minNode);
}

int HMMMinHeap::sink(int currentPos) {
  std::unique_ptr<HMMNode> nodeToSink = std::move(hmm_nodes[currentPos]);
  int son = currentPos * 2;
  bool isNotHeap = true;
  while (son <= size && isNotHeap) {
    if (son < size &&
        hmm_nodes[son + 1]->getLogProb() < hmm_nodes[son]->getLogProb()) {
      son++;
    }

    if (hmm_nodes[son]->getLogProb() < nodeToSink->getLogProb()) {
      HMMActives[hmm_nodes[son]->getId()] = currentPos;
      hmm_nodes[currentPos] = std::move(hmm_nodes[son]);
      currentPos = son;
      son = currentPos * 2;
    } else {
      isNotHeap = false;
    }
  }

  hmm_nodes[currentPos] = std::move(nodeToSink);
  HMMActives[hmm_nodes[currentPos]->getId()] = currentPos;
}

void HMMMinHeap::expandCapacity() {
  capacity *= 2;
  std::vector<std::unique_ptr<HMMNode>> hmm_nodes_tmp(capacity);
  for (size_t i = 1; i < size + 1; i++) {
    hmm_nodes_tmp[i] = std::move(hmm_nodes[i]);
  }
  hmm_nodes.swap(hmm_nodes_tmp);
}

int HMMMinHeap::bubbleUp(const std::unique_ptr<HMMNode>& hmm_node,
                         int position) {
  while (position > 1 &&
         hmm_node->getLogProb() < hmm_nodes[position / 2]->getLogProb()) {
    HMMActives[hmm_nodes[position / 2]->getId()] = position;
    hmm_nodes[position / 2].swap(hmm_nodes[position]);
    position = position / 2;
  }
  return position;
}

int HMMMinHeap::getNodePositionById(const uint32_t sg_state,
                                    const uint32_t hmm_q_state) {
  return HMMActives[HMMNodeId(sg_state, hmm_q_state)];
}

std::unique_ptr<HMMNode>& HMMMinHeap::getNodeAtPosition(int position) {
  return hmm_nodes[position];
}

int HMMMinHeap::insert(std::unique_ptr<HMMNode> hmm_node) {
  if (size == hmm_nodes.size() - 1) {
    expandCapacity();
  }
  int position = size + 1;
  assert(size <= capacity);

  if (size != 1) {
    position = bubbleUp(hmm_node, position);
  }
  size++;
  hmm_nodes[position] = std::move(hmm_node);
  HMMActives[hmm_nodes[position]->getId()] = position;
  return position;
}

std::unique_ptr<HMMNode> HMMMinHeap::popAndInsert(
    std::unique_ptr<HMMNode> hmm_node) {
  HMMActives[hmm_nodes[1]->getId()] = -1;
  HMMActives[hmm_node->getId()] = 1;
  std::unique_ptr<HMMNode> minNode = std::move(hmm_nodes[1]);
  hmm_nodes[1] = std::move(hmm_node);
  if (size > 1) sink(1);
  return std::move(minNode);
}

int HMMMinHeap::update(const uint32_t sg_state, const uint32_t hmm_q_state,
                       float lprob) {
  int position = HMMActives[HMMNodeId(sg_state, hmm_q_state)];
  hmm_nodes[position]->setHMMLogProb(lprob);
  hmm_nodes[position]->setLMLogProb(lprob);
  hmm_nodes[position]->setLogprob(lprob);

  std::unique_ptr<HMMNode> hmm_node = std::move(hmm_nodes[position]);
  position = bubbleUp(hmm_node, position);
  hmm_nodes[position] = std::move(hmm_node);
  HMMActives[hmm_nodes[position]->getId()] = position;
}

void HMMMinHeap::showHeapContent() {
  for (size_t i = 1; i < size + 1; i++) {
    std::cout << "Position: " << i << std::endl;
    hmm_nodes[i]->showHMMState();
    std::cout << "Position according to hash: "
              << HMMActives[hmm_nodes[i]->getId()] << std::endl;
  }
}

HMMNodeManager::HMMNodeManager(const uint32_t max_size)
    : max_hyps(max_size), max_size(max_size + 1), size(0) {}
