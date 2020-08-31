#include "DGaussianAcousticModel.h"

void GaussianState::addMu(const std::string &line) {
  mu = read_vector<float>(line);
}

void GaussianState::addVar(const std::string &line) {
  var = read_vector<float>(line);

  float lgc = 0.0f;

  for (auto value : var) {
    ivar.push_back(1.0 / value);
    lgc += log(value);
  }
  lgc += var.size() * LOG2PI;

  logc = -0.5 * lgc;
}

void GaussianState::print_state() {
  print_vector<float>(mu);
  print_vector<float>(var);
  std::cout << logc << std::endl;
}

float GaussianState::calc_prob(const std::vector<float> &frame) {
  float prob = 0.0;
  float aux = 0.0;

  for (auto i = 0; i < frame.size(); i++) {
    aux = frame[i] - mu[i];
    prob += (aux * aux) * ivar[i];
  }
  return -0.5 * prob + logc;
}

int DGaussianAcousticModel::read_model(const std::string &filename) {
  std::cout << "Reading DGaussianAcoustic model from " << filename << "..."
            << std::endl;

  std::ifstream fileI(filename, std::ifstream::in);
  std::string line;
  const char del = ' ';

  if (fileI.is_open()) {
    getline(fileI, line);       // AMODEL
    getline(fileI, line);       // DGaussian
    getline(fileI, line, del);  // D <dim>
    getline(fileI, line);       // D <dim>
    std::stringstream(line) >> dim;

    getline(fileI, line, del);  // SMOOTH
    getline(fileI, line);       // Value

    smooth = parse_line(line);

    getline(fileI, line, del);  // N
    getline(fileI, line);
    std::stringstream(line) >> n_states;

    std::string name;
    int n_q;

    for (int statesIter = 0; statesIter < n_states; statesIter++) {
      std::vector<float> trans;

      // Name
      getline(fileI, line);
      line.erase(0, 1);
      line.erase(line.size() - 1, line.size());
      std::stringstream(line) >> name;
      states.push_back(name);

      // Q
      getline(fileI, line, del);
      getline(fileI, line);
      std::stringstream(line) >> n_q;

      state_to_num_q[name] = n_q;

      getline(fileI, line);  // Trans
      getline(fileI, line);
      trans = parse_line(line);

      state_to_trans[name] = trans;

      std::vector<GaussianState> gstates;

      for (int i = 0; i < n_q; i++) {
        GaussianState gstate;

        // MU
        getline(fileI, line, del);  // MU
        getline(fileI, line);       // values

        gstate.addMu(line);

        getline(fileI, line, del);  // VAR
        getline(fileI, line);       // values

        gstate.addVar(line);
        gstates.push_back(gstate);
      }

      state_to_gstate[name] = gstates;
    }

    fileI.close();
  } else {
    std::cout << "Unable to open the file " << filename << " for reading."
              << std::endl;
    return 1;
  }
  return 0;
}

int DGaussianAcousticModel::write_model(const std::string &filename) {
  std::ofstream fileO(filename, std::ios::app);

  if (fileO.is_open()) {
    int n_q;

    fileO << "AMODEL\n";
    fileO << "DGaussian\n";
    fileO << "D " << dim << std::endl;
    fileO << "SMOOTH ";

    for (auto &value : smooth) fileO << value << " ";

    fileO << "\n";
    fileO << "N " << n_states << std::endl;

    for (auto &name : states) {
      n_q = state_to_num_q[name];

      fileO << "'" << name << "'" << std::endl;
      fileO << "Q " << n_q << std::endl;

      fileO << "Trans\n";

      for (auto &value : state_to_trans[name]) fileO << value << " ";

      fileO << std::endl;

      std::vector<GaussianState> gstates = state_to_gstate[name];

      for (auto i = 0; i < n_q; i++) {
        fileO << "MU ";
        for (auto &value : gstates[i].getMu()) fileO << value << " ";

        fileO << std::endl;

        fileO << "VAR ";
        for (auto &value : gstates[i].getVar()) fileO << value << " ";

        fileO << std::endl;
      }
    }

    fileO.close();

  } else {
    std::cout << "Unable to open file for writing" << std::endl;
    return 1;
  }
  return 0;
}

DGaussianAcousticModel::DGaussianAcousticModel(const std::string &filename) {
  DGaussianAcousticModel::read_model(filename);
}

unsigned int DGaussianAcousticModel::getDim() { return dim; }

unsigned int DGaussianAcousticModel::getNStates() { return n_states; }

float DGaussianAcousticModel::calc_prob(const std::string &state, int q,
                                        const std::vector<float> &frame) {
  std::vector<GaussianState> gstates = state_to_gstate[state];
  return gstates[q].calc_prob(frame);
}

