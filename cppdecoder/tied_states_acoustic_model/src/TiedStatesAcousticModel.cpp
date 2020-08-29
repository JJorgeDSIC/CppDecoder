#include "TiedStatesAcousticModel.h"

#include "Utils.h"

int TiedStatesAcousticModel::read_model(const std::string &filename) {
  std::cout << "Reading TiedStatesAcousticModel from " << filename << "..."
            << std::endl;

  std::ifstream fileI(filename, std::ifstream::in);
  std::string line, name;
  const char del = ' ';

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

    for (auto i = 0; i < n_states; i++) {
      std::vector<float> pmembers;

      getline(fileI, line);  // state_id
      std::stringstream(line) >> name;
      senones.push_back(name);

      getline(fileI, line, del);  // I
      getline(fileI, line);       // value
      std::stringstream(line) >> components;

      getline(fileI, line, del);  // PMembers
      getline(fileI, line);       // value
      pmembers = parse_line(line);

      getline(fileI, line);  // Members

      std::vector<std::vector<float>> states_mu;
      std::vector<std::vector<float>> states_var;
      std::vector<std::vector<float>> states_ivar;
      std::vector<float> states_logc;

      for (auto i = 0; i < components; i++) {
        std::vector<float> mu;
        std::vector<float> var;
        std::vector<float> ivar;
        float logc = 0;

        // MU
        getline(fileI, line, del);  // MU
        getline(fileI, line);       // values

        mu = parse_line(line);

        getline(fileI, line, del);  // VAR
        getline(fileI, line);       // values

        var = parse_line(line);

        for (const auto &value : var) {
          ivar.push_back(1.0 / value);
          logc += log(value);
        }

        logc += dim * LOG2PI;

        logc = -0.5 * logc;

        states_mu.push_back(mu);
        states_var.push_back(var);
        states_ivar.push_back(ivar);
        states_logc.push_back(logc);
      }

      senone_to_mus[name] = states_mu;
      senone_to_vars[name] = states_var;
      senone_to_ivars[name] = states_ivar;
      senone_to_logc[name] = states_logc;
      senone_to_pmembers[name] = pmembers;
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
      std::vector<float> pmembers = senone_to_pmembers[senones[i]];
      int components = pmembers.size();

      fileO << "I " << components << std::endl;
      fileO << "PMembers ";
      for (const auto &value : pmembers) {
        fileO << value << " ";
      }
      fileO << std::endl;

      fileO << "Members" << std::endl;

      for (auto j = 0; j < components; j++) {
        std::vector<float> mu = senone_to_mus[senones[i]][j];
        fileO << "MU ";
        for (const auto &value : mu) fileO << value << " ";
        fileO << std::endl;

        std::vector<float> var = senone_to_vars[senones[i]][j];
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

int main() {
  TiedStatesAcousticModel amodel("bin/models/tiedphoneme_I04.example.model");
  amodel.write_model("bin/models/tiedphoneme_I04.example.again.model");
}
