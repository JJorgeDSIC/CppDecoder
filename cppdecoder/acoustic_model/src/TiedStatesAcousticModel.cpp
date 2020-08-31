#include "TiedStatesAcousticModel.h"

int TiedStatesAcousticModel::read_model(const std::string &filename) {
  std::cout << "Reading TiedStatesAcousticModel from " << filename << "..."
            << std::endl;

  std::ifstream fileI(filename, std::ifstream::in);
  std::string line, name;
  const char del = ' ';
  size_t i, statesIter;

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

    smooth = parse_line(line);

    getline(fileI, line, del);  // N
    getline(fileI, line);       // Value
    std::stringstream(line) >> n_states;

    getline(fileI, line);  // States

    for (statesIter = 0; statesIter < n_states; statesIter++) {
      GaussianMixtureState dg_state;

      getline(fileI, line);  // state_id
      std::stringstream(line) >> name;
      senones.push_back(name);

      getline(fileI, line, del);  // I
      getline(fileI, line);       // value
      std::stringstream(line) >> components;
      dg_state.setComponents(components);

      getline(fileI, line, del);  // PMembers
      getline(fileI, line);       // value
      dg_state.addPMembers(line);

      getline(fileI, line);  // Members

      for (auto j = 0; j < components; j++) {
        GaussianState gstate;

        getline(fileI, line, del);  // MU
        getline(fileI, line);       // values
        gstate.addMu(line);

        getline(fileI, line, del);  // VAR
        getline(fileI, line);       // values

        gstate.addVar(line);

        dg_state.addGaussianState(gstate);
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
    for (const auto &value : smooth) {
      fileO << value << " ";
    }
    fileO << std::endl;

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
      for (const auto &value : pmembers) {
        fileO << value << " ";
      }
      fileO << std::endl;

      fileO << "Members" << std::endl;
      GaussianMixtureState gsmixstate = senone_to_mixturestate[senones[i]];
      for (auto j = 0; j < components; j++) {
        GaussianState gstate = gsmixstate.getGaussianStateByComponent(j);
        std::vector<float> mu = gstate.getMu();
        fileO << "MU ";
        for (const auto &value : mu) fileO << value << " ";
        fileO << std::endl;

        std::vector<float> var = gstate.getVar();
        fileO << "VAR ";
        for (const auto &value : var) fileO << value << " ";
        fileO << std::endl;
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
        for (const auto &value : trans) fileO << value << " ";
        fileO << std::endl;

      } else {
        fileO << "TransP " << symbol_equivalent << std::endl;
      }

      std::vector<std::string> senones = symbol_to_senones[symbol];

      for (const auto &value : senones) fileO << value << " ";
      fileO << std::endl;
    }

  } else {
    std::cout << "Unable to open file for writing." << std::endl;
    return 1;
  }
  return 0;
}

TiedStatesAcousticModel::TiedStatesAcousticModel(const std::string &filename) {
  TiedStatesAcousticModel::read_model(filename);
}

float TiedStatesAcousticModel::calc_prob(const std::string &state, int q,
                                         const std::vector<float> &frame) {
  std::vector<std::string> senones = symbol_to_senones[state];
  std::string senon = senones[q];

  if (q > senones.size()) return -1.0;

  GaussianMixtureState dgstate = senone_to_mixturestate[senon];

  int components = dgstate.getComponents();

  std::vector<float> pmembers = dgstate.getPMembers();
  std::vector<float> pprob;

  float max = -HUGE_VAL;

  for (auto i = 0; i < components; i++) {
    float prob = 0.0, aux = 0.0;

    prob = dgstate.getGaussianStateByComponent(i).calc_prob(frame);

    aux = pmembers[i] + prob;
    if (aux > max) max = aux;

    pprob.push_back(aux);
  }

  if (max != -HUGE_VAL) {
    float r_add = robust_add(pprob, max, components);
    return r_add;
  } else {
    return -HUGE_VAL;
  }
}
