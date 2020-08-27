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

const float LOG2PI = 1.83787706641f;

class TiedStatesAcousticModel {
  unsigned int dim;
  unsigned int n_states;
  unsigned int n_trans;
  std::vector<float> smooth;

  std::vector<std::string> symbols;
  std::vector<std::string> senones;

  std::unordered_map<std::string, std::vector<std::string>> symbol_to_senone;

  std::unordered_map<std::string, std::vector<std::vector<float>>>
      senone_to_mus;
  std::unordered_map<std::string, std::vector<std::vector<float>>>
      senone_to_vars;
  std::unordered_map<std::string, std::vector<std::vector<float>>>
      senone_to_ivars;
  std::unordered_map<std::string, std::vector<float>> senone_to_logc;
  std::unordered_map<std::string, std::vector<float>> senone_to_pmembers;
  std::unordered_map<std::string, std::string> symbol_to_symbol_transitions;
  std::unordered_map<std::string, std::vector<float>> symbol_to_transitions;
  std::unordered_map<std::string, std::vector<std::string>> symbol_to_senones;

 public:
  TiedStatesAcousticModel();
  TiedStatesAcousticModel(const std::string &filename);
  ~TiedStatesAcousticModel();
  void read_model(const std::string &filename);
  void write_model(const std::string &filename);
};

#endif  // TIEDSTATESACOUSTICMODEL_H_
