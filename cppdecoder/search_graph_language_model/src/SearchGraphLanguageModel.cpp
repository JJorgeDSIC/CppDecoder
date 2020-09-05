/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include "SearchGraphLanguageModel.h"

#include <Utils.h>

int SearchGraphLanguageModel::write_model(const std::string &filename) {
  std::cout << "Writing model in " << filename << std::endl;
  std::ofstream fileO(filename, std::ios::app);
  if (fileO.is_open()) {
    fileO << "SG" << std::endl;
    fileO << "NStates " << nstates << std::endl;
    fileO << "NEdges " << nedges << std::endl;
    fileO << "Start " << start << std::endl;
    fileO << "Final " << final << std::endl;
    fileO << "States" << std::endl;

    for (SearchGraphLanguageModelState state : sg_lm_states) {
      fileO << state.id << " ";
      fileO << state.symbol << " ";
      fileO << state.word << " ";
      fileO << state.edge_begin << " ";
      fileO << state.edge_end << std::endl;
    }

    fileO << "Edges" << std::endl;
    for (SearchGraphLanguageModelEdge edge : sg_lm_edges) {
      fileO << edge.id << " ";
      fileO << edge.dst << " ";
      fileO << edge.weight << std::endl;
    }

  } else {
    std::cout << "Unable to open file for writing" << std::endl;
    return 1;
  }
  return 0;
}

int SearchGraphLanguageModel::read_model(const std::string &filename) {
  std::cout << "Reading language model..." << std::endl;

  std::ifstream fileI(filename, std::ifstream::in);
  std::string line;
  const char del = ' ';

  if (fileI.is_open()) {
    getline(fileI, line);  // SG

    nstates = read_header_line(fileI, line, del);
    nedges = read_header_line(fileI, line, del);
    start = read_header_line(fileI, line, del);
    final = read_header_line(fileI, line, del);

    getline(fileI, line);  // States

    int state_id, edge_begin, edge_end;

    std::string symbol, word;

    for (size_t i = 0; i < nstates; i++) {
      getline(fileI, line);
      std::istringstream ss(line);
      ss >> state_id;
      ss >> symbol;
      ss >> word;
      ss >> edge_begin;
      ss >> edge_end;

      SearchGraphLanguageModelState state(state_id, symbol, word, edge_begin,
                                          edge_end);

      sg_lm_states.push_back(state);
    }

    getline(fileI, line);  // Edges
    int id, dst;
    float weight;

    for (size_t i = 0; i < nedges; i++) {
      getline(fileI, line);
      std::istringstream ss(line);
      ss >> id;
      ss >> dst;
      ss >> weight;

      SearchGraphLanguageModelEdge e(id, dst, weight);

      sg_lm_edges.push_back(e);
    }

  } else {
    std::cout << "Unable to open the file " << filename << " for reading."
              << std::endl;
    return 1;
  }
  return 0;
}
