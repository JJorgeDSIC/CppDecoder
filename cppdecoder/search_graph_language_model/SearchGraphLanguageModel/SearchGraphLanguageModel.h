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
                                std::string word, int edge_begin, int edge_end);
};

class SearchGraphLanguageModelEdge {
 public:
  int edge_id;
  int edge_dst;
  float edge_weight;

  SearchGraphLanguageModelEdge(int edge_id, int edge_dst, float edge_weight);
};

class SearchGraphLanguageModel {
  std::unordered_map<std::string, int> symbol_to_id;
  std::unordered_map<int, std::string> id_to_symbol;

  std::unordered_map<std::string, int> word_to_id;
  std::unordered_map<int, std::string> id_to_word;

  int symbol_id = 0;
  int word_id = 0;

  int nstates, nedges, start, final;

  std::vector<SearchGraphLanguageModelState> sg_lm_states;
  std::vector<SearchGraphLanguageModelEdge> sg_lm_edges;

 public:
  void read_model(const std::string &filename);
  void write_model(const std::string &filename);
};