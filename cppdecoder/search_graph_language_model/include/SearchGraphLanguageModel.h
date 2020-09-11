/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef SEARCHGRAPHLANGUAGEMODEL_H_
#define SEARCHGRAPHLANGUAGEMODEL_H_

#include <Utils.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct SearchGraphLanguageModelState {
  int id;
  std::string symbol;
  std::string word;
  int edge_begin, edge_end;
};

struct SearchGraphLanguageModelEdge {
  int id;
  int dst;
  float weight;
};

class SearchGraphLanguageModel {
 public:
  SearchGraphLanguageModel();

  int read_model(const std::string& filename);

  int write_model(const std::string& filename);

  const std::string& getIdToSym(const int id) { return id_to_symbol[id]; }
  const std::string& getIdToWord(const int id) { return id_to_word[id]; }
  const SearchGraphLanguageModelState& getSearchGraphState(const size_t id) {
    return sg_lm_states[id];
  }
  const SearchGraphLanguageModelEdge& getSearchGraphEdge(const size_t id) {
    return sg_lm_edges[id];
  }
  size_t getNStates() const { return nstates; }
  size_t getNEdges() const { return nedges; }
  size_t getStartState() const { return start; }
  size_t getFinalState() const { return final; }

 private:
  std::unordered_map<std::string, int> symbol_to_id;
  std::unordered_map<int, std::string> id_to_symbol;

  std::unordered_map<std::string, int> word_to_id;
  std::unordered_map<int, std::string> id_to_word;

  size_t nstates, nedges, start, final;

  std::vector<SearchGraphLanguageModelState> sg_lm_states;
  std::vector<SearchGraphLanguageModelEdge> sg_lm_edges;
};

#endif  // SEARCHGRAPHLANGUAGEMODEL_H_
