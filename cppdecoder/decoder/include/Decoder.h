/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef DECODER_H_
#define DECODER_H_

#include <AcousticModel.h>
#include <Sample.h>
#include <SearchGraphLanguageModel.h>

#include <memory>

class SGNode {
  int state_id = 0;
  float lprob = 0.0;
  float hmmlprob = 0.0;
  float lmlprob = 0;
  int hyp = 0;

 public:
  SGNode(int state_id, float lprob, float hmmlprob, float lmlprob, int hyp)
      : state_id(state_id),
        lprob(lprob),
        hmmlprob(hmmlprob),
        lmlprob(lmlprob),
        hyp(hyp) {}
  int getStateId() { return state_id; }
  bool operator==(const SGNode& p) const { return state_id == p.state_id; }
};

class HMMNode {
  size_t s;
};

class WordHyp {
  int prev;
  std::string word;

 public:
  WordHyp(int prev, std::string word) : prev(prev), word(word) {}
  void showWordHyp() {
    std::cout << "Prev: " << prev << " Word: " << word << std::endl;
  }
};

class HMMNodeManager {
  size_t max_hyps;
  size_t max_size;
  size_t size;

 public:
  HMMNodeManager(size_t max_size)
      : max_hyps(max_size), max_size(max_size + 1), size(0) {}
  bool isFull();
};

class Decoder {
  std::unique_ptr<SearchGraphLanguageModel> sgraph;
  std::unique_ptr<AcousticModel> amodel;
  std::unique_ptr<SGNode> searchgraph_null_nodes0;
  std::vector<int> actives;
  std::vector<std::unique_ptr<SGNode>> search_graph_null_nodes0;
  std::vector<std::unique_ptr<SGNode>> search_graph_null_nodes1;
  std::vector<std::unique_ptr<SGNode>> search_graph_nodes1;

 public:
  Decoder(std::unique_ptr<SearchGraphLanguageModel>& sgraph,
          std::unique_ptr<AcousticModel>& amodel);
  float decode(Sample sample);
  void expand_search_graph_nodes(std::vector<SGNode>& searchgraph_null_nodes0);
  // void insert_search_graph_node(std::unique_ptr<SGNode>& node);
  void insert_search_graph_node(std::unique_ptr<SGNode>& node);
  void addNodeToSearchGraphNullNodes1(std::unique_ptr<SGNode>& node);
  void addNodeToSearchGraphNodes1(SGNode node);
};

#endif  // MIXTUREACOUSTICMODEL_H_
