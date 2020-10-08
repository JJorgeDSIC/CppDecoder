/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include "DGaussianAcousticModel.h"

GaussianState::GaussianState(const uint32_t d) : dim(d), logc(0) {
  mu.reserve(dim);
  var.reserve(dim);
  ivar.reserve(dim);
}

GaussianState::GaussianState(const uint32_t d, const std::string &mu_line,
                             const std::string &var_line)
    : dim(d), logc(0) {
  mu.reserve(dim);
  var.reserve(dim);
  ivar.reserve(dim);
  addMu(mu_line);
  addVar(var_line);
}

void GaussianState::addMu(const std::string &line) {
  mu = read_vector<float>(line);
}

void GaussianState::addVar(const std::string &line) {
  var = read_vector<float>(line);

  float lgc = 0.0f;

  for (auto value : var) {
    ivar.emplace_back(1.0 / value);
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

float GaussianState::calc_logprob(const std::vector<float> &frame) {
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

    smooth = read_vector<float>(line);

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
      trans = read_vector<float>(line);

      state_to_trans[name] = trans;

      std::vector<std::unique_ptr<GaussianState>> gstates;

      gstates.reserve(n_q);

      std::string mu_line, var_line;

      for (int i = 0; i < n_q; i++) {
        getline(fileI, line, del);  // MU
        getline(fileI, mu_line);    // values

        getline(fileI, line, del);  // VAR
        getline(fileI, var_line);   // values

        gstates.emplace_back(new GaussianState(dim, mu_line, var_line));
      }

      state_to_gstate[name] = std::move(gstates);
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
    fileO << "AMODEL\n";
    fileO << "DGaussian\n";
    fileO << "D " << dim << std::endl;
    fileO << "SMOOTH ";
    for (uint32_t i = 0; i < smooth.size() - 1; i++) {
      fileO << smooth[i] << " ";
    }

    fileO << smooth[smooth.size() - 1] << std::endl;

    fileO << "N " << n_states << std::endl;

    for (auto &name : states) {
      int n_q = state_to_num_q[name];

      fileO << "'" << name << "'" << std::endl;
      fileO << "Q " << n_q << std::endl;

      fileO << "Trans\n";

      std::vector<float> trans = state_to_trans[name];

      for (uint32_t i = 0; i < trans.size() - 1; i++) {
        fileO << trans[i] << " ";
      }

      fileO << trans[trans.size() - 1] << std::endl;

      auto it = state_to_gstate.find(name);

      for (auto i = 0; i < n_q; i++) {
        fileO << "MU ";
        std::vector<float> mu = it->second[i]->getMu();
        for (uint32_t i = 0; i < mu.size() - 1; i++) {
          fileO << mu[i] << " ";
        }

        fileO << mu[mu.size() - 1] << std::endl;

        fileO << "VAR ";
        std::vector<float> var = it->second[i]->getVar();

        for (uint32_t i = 0; i < var.size() - 1; i++) {
          fileO << var[i] << " ";
        }

        fileO << var[var.size() - 1] << std::endl;
      }
    }

    fileO.close();

  } else {
    std::cout << "Unable to open file for writing" << std::endl;
    return 1;
  }
  return 0;
  
}

DGaussianAcousticModel::DGaussianAcousticModel(const std::string &filename)
    : AcousticModel() {
  DGaussianAcousticModel::read_model(filename);
}

float DGaussianAcousticModel::calc_logprob(const std::string &state,
                                           const int q,
                                           const std::vector<float> &frame) {
  auto it = state_to_gstate.find(state);
  if (it != state_to_gstate.end()) {
    if (it->second.size() < q || frame.size() != it->second[q]->getDim())
      return INFINITY;
    return it->second[q]->calc_logprob(frame);
  } else {
    return INFINITY;
  }
}
