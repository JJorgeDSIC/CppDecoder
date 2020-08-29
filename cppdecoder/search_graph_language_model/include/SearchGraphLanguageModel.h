/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef SEARCHGRAPHLANGUAGEMODEL_H_
#define SEARCHGRAPHLANGUAGEMODEL_H_

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
  int state_id;
  std::string symbol;
  std::string word;
  int edge_begin, edge_end;

  SearchGraphLanguageModelState(int state_id, std::string symbol,
                                std::string word, int edge_begin, int edge_end)
      : state_id(state_id),
        symbol(symbol),
        word(word),
        edge_begin(edge_begin),
        edge_end(edge_end) {}
};

class SearchGraphLanguageModelEdge {
 public:
  int edge_id;
  int edge_dst;
  float edge_weight;

  SearchGraphLanguageModelEdge(int edge_id, int edge_dst, float edge_weight)
      : edge_id(edge_id), edge_dst(edge_dst), edge_weight(edge_weight) {}
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
  int read_model(const std::string &filename);
  int write_model(const std::string &filename);
};

#endif  // SEARCHGRAPHLANGUAGEMODEL_H_
