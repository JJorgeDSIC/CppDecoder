/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include <HMM.h>

HMMNodeId::HMMNodeId() {
  this->sg_state = 0;
  this->hmm_q_state = 0;
}

HMMNode::HMMNode()
    : lprob(0.0), hmmlp(0.0), lmlp(0.0), trapos(0), hyp_index(0) {
  this->id = HMMNodeId(0, 0);
}

HMMNode::HMMNode(const uint32_t sg_state, const uint32_t hmm_q_state)
    : lprob(0.0), hmmlp(0.0), lmlp(0.0), trapos(0), hyp_index(0) {
  this->id = HMMNodeId(sg_state, hmm_q_state);
}

HMMNode::HMMNode(const uint32_t sg_state, const uint32_t hmm_q_state,
                 const float lprob, const float hmmlp, const float lmlp,
                 const uint32_t trapos, const uint32_t hyp_index)
    : lprob(lprob),
      hmmlp(hmmlp),
      lmlp(lmlp),
      trapos(trapos),
      hyp_index(hyp_index) {
  this->id = HMMNodeId(sg_state, hmm_q_state);
}

void HMMNode::showHMMState() {
  std::cout << "HMM ID_S: " << id.sg_state << ", ID_Q: " << id.hmm_q_state
            << ", LProb: " << lprob << ", HMMProb: " << hmmlp
            << ", LMProb: " << lmlp << ", Trapos: " << trapos
            << ", h: " << hyp_index << std::endl;
}

float HMMMinHeap::getMinLProb() {
  assert(size > 0);
  return hmm_nodes[1]->getLogProb();
}

std::unique_ptr<HMMNode> HMMMinHeap::extractMinLProbHMMNode() {
  HMMActives[hmm_nodes[1]->getId()] = 0;
  HMMActives[hmm_nodes[size]->getId()] = 1;

  hmm_nodes[1].swap(hmm_nodes[size--]);

  std::unique_ptr<HMMNode> minNode = std::move(hmm_nodes[size + 1]);
  if (size > 1) sink(1);
  return std::move(minNode);
}

int HMMMinHeap::sink(int currentPos) {
  std::unique_ptr<HMMNode> nodeToSink = std::move(hmm_nodes[currentPos]);
  int prevPos = currentPos;
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
  return currentPos;
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

std::unique_ptr<HMMNode>& HMMMinHeap::getNodeById(const uint32_t sg_state,
                                                  const uint32_t hmm_q_state) {
  return hmm_nodes[getNodePositionById(sg_state, hmm_q_state)];
}

int HMMMinHeap::insert(std::unique_ptr<HMMNode> hmm_node) {
  if (size == hmm_nodes.size() - 1) {
    expandCapacity();
  }
  int position = size + 1;
  assert(size <= capacity);

  if (size != 0) {
    position = bubbleUp(hmm_node, position);
  }
  size++;
  hmm_nodes[position] = std::move(hmm_node);
  HMMActives[hmm_nodes[position]->getId()] = position;
  return position;
}

std::unique_ptr<HMMNode> HMMMinHeap::popAndInsert(
    std::unique_ptr<HMMNode> hmm_node) {
  HMMActives[hmm_nodes[1]->getId()] = 0;
  HMMActives[hmm_node->getId()] = 1;
  std::unique_ptr<HMMNode> minNode = std::move(hmm_nodes[1]);
  hmm_nodes[1] = std::move(hmm_node);
  if (size > 1) sink(1);
  return std::move(minNode);
}

int HMMMinHeap::updateNodeAt(int position, float lprob, float hmmlp) {
  hmm_nodes[position]->setLogprob(lprob);
  hmm_nodes[position]->setHMMLogProb(hmmlp);
  // TODO: Prepare a test for this
  position = sink(position);
}

void HMMMinHeap::exchangeActives(
    const std::unique_ptr<HMMMinHeap>& hmm_min_heap_other) {
  HMMActives.swap(hmm_min_heap_other->HMMActives);
}

void HMMMinHeap::exchangeNodes(
    const std::unique_ptr<HMMMinHeap>& hmm_min_heap_other) {
  hmm_nodes.swap(hmm_min_heap_other->hmm_nodes);
}

void HMMMinHeap::showHeapContent() {
  for (size_t i = 1; i < size + 1; i++) {
    hmm_nodes[i]->showHMMState();
  }
}

void HMMMinHeap::cleanActives() {
  // TODO: Faster solution?
  HMMActives.clear();
}

void HMMMinHeap::reset() {
  size = 0;
  hmm_nodes.clear();
  HMMActives.clear();
}