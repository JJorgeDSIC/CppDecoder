/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef DGAUSSIANACOUSTICMODEL_H_
#define DGAUSSIANACOUSTICMODEL_H_

#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Utils.h"

class GaussianState {
  std::vector<float> mu;
  std::vector<float> var;
  std::vector<float> ivar;
  float logc;

 public:
  void addMu(const std::string &line);
  void addVar(const std::string &line);
  void setLogc(float logc) { this->logc = logc; }
  std::vector<float> &getMu() { return mu; }
  std::vector<float> &getVar() { return var; }
  std::vector<float> &getIVar() { return ivar; }
  float &getLogc() { return logc; }
  void print_state();
  float calc_logprob(const std::vector<float> &frame);
};

class DGaussianAcousticModel {
  std::vector<std::string> states;
  std::unordered_map<std::string, std::vector<GaussianState>> state_to_gstate;
  std::unordered_map<std::string, std::vector<float>> state_to_trans;
  std::unordered_map<std::string, int> state_to_num_q;
  std::vector<float> smooth;
  unsigned int dim;
  unsigned int n_states;

 public:
  explicit DGaussianAcousticModel(const std::string &filename);
  unsigned int getDim();
  unsigned int getNStates();
  int read_model(const std::string &filename);
  int write_model(const std::string &filename);
  float calc_logprob(const std::string &state, int q,
                  const std::vector<float> &frame);
};

#endif  // DGAUSSIANACOUSTICMODEL_H_
