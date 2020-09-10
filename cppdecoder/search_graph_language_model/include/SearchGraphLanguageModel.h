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

class SearchGraphLanguageModelState {
 public:
  int id;
  std::string symbol;
  std::string word;
  int edge_begin, edge_end;

  SearchGraphLanguageModelState(int id, std::string symbol, std::string word,
                                int edge_begin, int edge_end)
      : id(id),
        symbol(symbol),
        word(word),
        edge_begin(edge_begin),
        edge_end(edge_end) {}
};

class SearchGraphLanguageModelEdge {
 public:
  int id;
  int dst;
  float weight;

  SearchGraphLanguageModelEdge(int id, int dst, float weight)
      : id(id), dst(dst), weight(weight) {}
};

class SearchGraphLanguageModel {
  std::unordered_map<std::string, int> symbol_to_id;
  std::unordered_map<int, std::string> id_to_symbol;

  std::unordered_map<std::string, int> word_to_id;
  std::unordered_map<int, std::string> id_to_word;

  size_t nstates, nedges, start, final;

  std::vector<SearchGraphLanguageModelState> sg_lm_states;
  std::vector<SearchGraphLanguageModelEdge> sg_lm_edges;

 public:
  SearchGraphLanguageModel() : nstates(0), nedges(0), start(-1), final(-1) {}
  int read_model(const std::string& filename);
  int write_model(const std::string& filename);
  int getNStates() { return nstates; }
  int getNEdges() { return nedges; }
  int getStartState() { return start; }
  int getFinalState() { return final; }
  const std::string& getIdToSym(int id) { return id_to_symbol[id]; }
  const std::string& getIdToWord(int id) { return id_to_word[id]; }
};

#endif  // SEARCHGRAPHLANGUAGEMODEL_H_
