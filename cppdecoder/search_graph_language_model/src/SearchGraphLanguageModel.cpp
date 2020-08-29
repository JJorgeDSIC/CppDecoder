#include "SearchGraphLanguageModel.h"

#include <Utils.h>

int read_header_line(std::ifstream &fileI, std::string line, const char del) {
  int value;
  getline(fileI, line, del);  // PARAM
  getline(fileI, line);       // values
  std::stringstream(line) >> value;
  return value;
}

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
      fileO << state.state_id << " ";
      fileO << state.symbol << " ";
      fileO << state.word << " ";
      fileO << state.edge_begin << " ";
      fileO << state.edge_end << std::endl;
    }

    fileO << "Edges" << std::endl;
    for (SearchGraphLanguageModelEdge edge : sg_lm_edges) {
      fileO << edge.edge_id << " ";
      fileO << edge.edge_dst << " ";
      fileO << edge.edge_weight << std::endl;
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
    int edge_id, edge_dst;
    float edge_weight;

    for (size_t i = 0; i < nedges; i++) {
      getline(fileI, line);
      std::istringstream ss(line);
      ss >> edge_id;
      ss >> edge_dst;
      ss >> edge_weight;

      SearchGraphLanguageModelEdge e(edge_id, edge_dst, edge_weight);

      sg_lm_edges.push_back(e);
    }

  } else {
    std::cout << "Unable to open the file " << filename << " for reading."
              << std::endl;
    return 1;
  }
  return 0;
}

int main() {
  std::cout << "Testing SearchGraphLanguageModel..." << std::endl;

  SearchGraphLanguageModel sgraph;
  sgraph.read_model("bin/models/2.gram.graph");
  sgraph.write_model("bin/models/2.gram.graph.again");
}
