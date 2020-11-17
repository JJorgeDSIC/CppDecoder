/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

inline void SGNode::showState() const {
  std::cout << "State_id " << state_id << ", lprob: " << lprob
            << ", hmmp: " << hmmlprob << ", hyp: " << hyp << std::endl;
}

inline void WordHyp::showWordHyp() const {
  std::cout << "Prev: " << prev << " Word: " << word << std::endl;
}

// inline void Decoder::printSGNodes(
//     const std::vector<std::unique_ptr<SGNode>>& nodes) {
//   std::cout << "printSGNodes: " << std::endl;
//   for (uint32_t i = 0; i < nodes.size(); i++) {
//     std::cout << nodes[i]->getStateId() << std::endl;
//     nodes[i]->showState();
//     std::cout << sgraph->getIdToSym(nodes[i]->getStateId()) << std::endl;
//     std::cout << sgraph->getIdToWord(nodes[i]->getStateId()) << std::endl;
//   }
// }