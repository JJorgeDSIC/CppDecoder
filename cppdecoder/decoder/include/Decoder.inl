/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

inline void SGNode::showState() {
  std::cout << state_id << " " << lprob << " " << std::endl;
}

inline void WordHyp::showWordHyp() {
  std::cout << "Prev: " << prev << " Word: " << word << std::endl;
}

inline void Decoder::printSGNode(
    const std::vector<std::unique_ptr<SGNode>>& nodes) {
  std::cout << "printSGNode" << std::endl;
  for (size_t i = 0; i < nodes.size(); i++) {
    std::cout << nodes[i]->getStateId() << std::endl;
    std::cout << sgraph->getIdToSym(nodes[i]->getStateId()) << std::endl;
    std::cout << sgraph->getIdToWord(nodes[i]->getStateId()) << std::endl;
  }
}