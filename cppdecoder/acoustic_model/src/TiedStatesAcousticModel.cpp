/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include "TiedStatesAcousticModel.h"

TiedStatesAcousticModel::TiedStatesAcousticModel(const std::string &filename)
    : AcousticModel() {
  TiedStatesAcousticModel::read_model(filename);
}

int TiedStatesAcousticModel::read_model(const std::string &filename) {
  std::cout << "Reading TiedStatesAcousticModel from " << filename << "..."
            << std::endl;

  std::ifstream fileI(filename, std::ifstream::in);
  std::string line, name;
  const char del = ' ';
  uint32_t i, statesIter;

  if (fileI.is_open()) {
    int components, n_q;

    getline(fileI, line);       // AMODEL
    getline(fileI, line);       // TiedStates
    getline(fileI, line);       // Mixture
    getline(fileI, line);       // DGaussian
    getline(fileI, line, del);  // D
    getline(fileI, line);       // DGaussian
    std::stringstream(line) >> dim;

    getline(fileI, line, del);  // SMOOTH
    getline(fileI, line);       // Value

    smooth = read_vector<float>(line);

    getline(fileI, line, del);  // N
    getline(fileI, line);       // Value
    std::stringstream(line) >> n_states;

    getline(fileI, line);  // States

    for (statesIter = 0; statesIter < n_states; statesIter++) {
      getline(fileI, line);  // state_id
      std::stringstream(line) >> name;
      senones.push_back(name);

      getline(fileI, line, del);  // I
      getline(fileI, line);       // value
      std::stringstream(line) >> components;

      GaussianMixtureState dg_state(components, dim);

      getline(fileI, line, del);  // PMembers
      getline(fileI, line);       // value
      dg_state.addPMembers(line);

      dg_state.setDim(dim);

      getline(fileI, line);  // Members

      std::string mu_line, var_line;

      for (auto j = 0; j < components; j++) {
        getline(fileI, line, del);  // MU
        getline(fileI, mu_line);    // values

        getline(fileI, line, del);  // VAR
        getline(fileI, var_line);   // values

        dg_state.addGaussianState(dim, mu_line, var_line);
      }

      senone_to_mixturestate[name] = dg_state;
    }

    getline(fileI, line, del);  // N
    getline(fileI, line);       // Value
    std::stringstream(line) >> n_trans;

    std::string symbol;

    for (auto i = 0; i < n_trans; i++) {
      getline(fileI, line);  //
      line.erase(0, 1);
      line.erase(line.size() - 1, line.size());
      std::stringstream(line) >> symbol;
      symbols.push_back(symbol);

      getline(fileI, line, del);  // Q
      getline(fileI, line);       // Value
      std::stringstream(line) >> n_q;

      getline(fileI, line);  // Trans / TransP

      std::vector<float> trans;
      std::vector<std::string> senones;

      if (line.find("TransP") == std::string::npos) {
        getline(fileI, line);

        std::stringstream strans(line);

        float x;

        while (strans >> x) trans.push_back(x);

        getline(fileI, line);

        std::stringstream ssenone(line);

        std::string s;

        while (ssenone >> s) senones.push_back(s);

        symbol_to_transitions[symbol] = trans;
        symbol_to_senones[symbol] = senones;
        symbol_to_symbol_transitions[symbol] = symbol;

      } else {
        std::string token;
        std::istringstream iss(line);
        iss >> token;  // TransP
        iss >> token;  // Symbol with transitions for this symbol

        getline(fileI, line);
        std::stringstream ssenone(line);

        std::string s;

        while (ssenone >> s) senones.push_back(s);

        symbol_to_transitions[symbol] = symbol_to_transitions[token];
        symbol_to_senones[symbol] = senones;
        symbol_to_symbol_transitions[symbol] = token;
      }
    }
    fileI.close();
  } else {
    std::cout << "Unable to open file for reading" << std::endl;
    return 1;
  }
  return 0;
}

int TiedStatesAcousticModel::write_model(const std::string &filename) {
  std::ofstream fileO(filename, std::ios::app);

  if (fileO.is_open()) {
    fileO << "AMODEL\n";
    fileO << "TiedStates\n";
    fileO << "Mixture\n";
    fileO << "DGaussian\n";
    fileO << "D " << dim << std::endl;
    fileO << "SMOOTH ";
    for (uint32_t i = 0; i < smooth.size() - 1; i++) {
      fileO << smooth[i] << " ";
    }

    fileO << smooth[smooth.size() - 1] << std::endl;

    fileO << "N ";
    fileO << n_states << std::endl;

    fileO << "States" << std::endl;

    for (auto i = 0; i < n_states; i++) {
      fileO << senones[i] << std::endl;
      std::vector<float> pmembers =
          senone_to_mixturestate[senones[i]].getPMembers();
      int components = pmembers.size();

      fileO << "I " << components << std::endl;
      fileO << "PMembers ";
      for (uint32_t i = 0; i < pmembers.size() - 1; i++) {
        fileO << pmembers[i] << " ";
      }

      fileO << pmembers[pmembers.size() - 1] << std::endl;

      fileO << "Members" << std::endl;

      GaussianMixtureState gsmixstate = senone_to_mixturestate[senones[i]];
      for (auto j = 0; j < components; j++) {
        std::vector<float> mu = gsmixstate.getMuByComponent(j);
        std::vector<float> var = gsmixstate.getVarByComponent(j);

        fileO << "MU ";
        for (uint32_t i = 0; i < mu.size() - 1; i++) {
          fileO << mu[i] << " ";
        }

        fileO << mu[mu.size() - 1] << std::endl;

        fileO << "VAR ";
        for (uint32_t i = 0; i < var.size() - 1; i++) {
          fileO << var[i] << " ";
        }

        fileO << var[var.size() - 1] << std::endl;
      }
    }

    fileO << "N " << n_trans << std::endl;

    for (auto i = 0; i < n_trans; i++) {
      std::string symbol = symbols[i];

      fileO << "'" << symbol << "'" << std::endl;

      fileO << "Q " << symbol_to_senones[symbol].size() << std::endl;

      std::string symbol_equivalent = symbol_to_symbol_transitions[symbol];

      if (symbol.compare(symbol_equivalent) == 0) {
        fileO << "Trans" << std::endl;
        std::vector<float> trans = symbol_to_transitions[symbol];
        for (uint32_t i = 0; i < trans.size() - 1; i++) {
          fileO << trans[i] << " ";
        }

        fileO << trans[trans.size() - 1] << std::endl;

      } else {
        fileO << "TransP " << symbol_equivalent << std::endl;
      }

      std::vector<std::string> senones = symbol_to_senones[symbol];

      for (uint32_t i = 0; i < senones.size() - 1; i++) {
        fileO << senones[i] << " ";
      }

      fileO << senones[senones.size() - 1] << std::endl;
    }

  } else {
    std::cout << "Unable to open file for writing." << std::endl;
    return 1;
  }
  return 0;
}

float TiedStatesAcousticModel::calc_logprob(const std::string &state,
                                            const int q,
                                            const std::vector<float> &frame) {
  std::vector<std::string> senones = symbol_to_senones[state];

  if (senones.size() == 0) return INFINITY;

  if (senones.size() < q) return INFINITY;

  std::string senon = senones[q];

  GaussianMixtureState dgstate = senone_to_mixturestate[senon];

  if (frame.size() != dgstate.getDim()) return INFINITY;

  return dgstate.calc_logprob(frame);
}
