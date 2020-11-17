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
  /**
   * @brief Construct a new Search Graph Language Model object
   *
   */
  SearchGraphLanguageModel();
  /**
   * @brief Reads a Search Graph Language model from disk
   *
   * @param[in] filename File location
   * @return int 0 if everything is OK, 1 if there was a problem.
   */
  int read_model(const std::string& filename);
  /**
   * @brief Write a Search Graph Language model from disk.
   *
   * @param[in] filename File location.
   * @return int 0 if everything is OK, 1 if there was a problem.
   */
  int write_model(const std::string& filename);
  /**
   * @brief Get the symbol with the provided id
   *
   * @param[in] id Symbol's id
   * @return const std::string& Symbol with this id
   */
  const std::string& getIdToSym(const int id) { return id_to_symbol[id]; }
  /**
   * @brief Get the word with the provided id
   *
   * @param[in] id word's id
   * @return const std::string& word with this id
   */
  const std::string& getIdToWord(const int id) { return id_to_word[id]; }
  /**
   * @brief Get the search graph state with the provided id
   *
   * @param[in] id search graph state's id
   * @return const SearchGraphLanguageModelState& search graph state with this
   * id
   */
  const SearchGraphLanguageModelState& getSearchGraphState(const uint32_t id) {
    return sg_lm_states[id];
  }
  /**
   * @brief Get the Search Graph Edge object with the provided id
   *
   * @param[in] id search graph edge's id
   * @return const SearchGraphLanguageModelEdge& search graph state with this id
   */
  const SearchGraphLanguageModelEdge& getSearchGraphEdge(const uint32_t id) {
    return sg_lm_edges[id];
  }
  /**
   * @brief Get the number of states (or nodes)
   *
   * @return uint32_t number of states (or nodes)
   */
  uint32_t getNStates() const { return nstates; }
  /**
   * @brief Get the number of edges
   *
   * @return uint32_t number of edges
   */
  uint32_t getNEdges() const { return nedges; }
  /**
   * @brief Get the initial state (start state)
   *
   * @return uint32_t index of the initial state
   */
  uint32_t getStartState() const { return start; }
  /**
   * @brief Get the final state
   *
   * @return uint32_t index of the final state
   */
  uint32_t getFinalState() const { return final; }

 private:
  std::unordered_map<std::string, int> symbol_to_id;
  std::unordered_map<int, std::string> id_to_symbol;

  std::unordered_map<std::string, int> word_to_id;
  std::unordered_map<int, std::string> id_to_word;

  uint32_t nstates, nedges, start, final;

  std::vector<SearchGraphLanguageModelState> sg_lm_states;
  std::vector<SearchGraphLanguageModelEdge> sg_lm_edges;
};

#endif  // SEARCHGRAPHLANGUAGEMODEL_H_
