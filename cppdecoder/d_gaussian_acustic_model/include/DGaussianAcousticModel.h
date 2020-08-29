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

const float LOG2PI = 1.83787706641f;

class DGaussianAcousticModel {
  std::vector<std::string> states;
  std::unordered_map<std::string, std::vector<std::vector<float>>> state_to_mus;
  std::unordered_map<std::string, std::vector<std::vector<float>>>
      state_to_vars;
  std::unordered_map<std::string, std::vector<std::vector<float>>>
      state_to_ivars;
  std::unordered_map<std::string, std::vector<float>> state_to_logc;
  std::unordered_map<std::string, std::vector<float>> state_to_trans;
  std::unordered_map<std::string, int> state_to_num_q;
  std::vector<float> smooth;
  unsigned int dim;
  unsigned int n_states;

 public:
  DGaussianAcousticModel() : n_states(0), dim(0) {}
  explicit DGaussianAcousticModel(const std::string &filename);
  ~DGaussianAcousticModel();

  unsigned int getDim();
  unsigned int getNStates();

  int read_model(const std::string &filename);
  int write_model(const std::string &filename);
  float calc_prob(const std::string &state, int q,
                  const std::vector<float> &frame);
};

#endif  // DGAUSSIANACOUSTICMODEL_H_
