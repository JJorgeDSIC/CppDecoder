/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef TIEDSTATESACOUSTICMODEL_H_
#define TIEDSTATESACOUSTICMODEL_H_

#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "DGaussianAcousticModel.h"
#include "MixtureAcousticModel.h"
#include "Utils.h"

class TiedStatesAcousticModel {
  size_t dim;
  size_t n_states;
  size_t n_trans;
  std::vector<float> smooth;

  std::vector<std::string> symbols;
  std::vector<std::string> senones;

  std::unordered_map<std::string, std::vector<std::string>> symbol_to_senone;
  std::unordered_map<std::string, GaussianMixtureState> senone_to_mixturestate;
  std::unordered_map<std::string, std::string> symbol_to_symbol_transitions;
  std::unordered_map<std::string, std::vector<float>> symbol_to_transitions;
  std::unordered_map<std::string, std::vector<std::string>> symbol_to_senones;

 public:
  explicit TiedStatesAcousticModel(const std::string &filename);
  int read_model(const std::string &filename);
  int write_model(const std::string &filename);
  float calc_logprob(const std::string &state, int q,
                  const std::vector<float> &frame);
};

#endif  // TIEDSTATESACOUSTICMODEL_H_
