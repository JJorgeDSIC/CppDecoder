/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef MIXTUREACOUSTICMODEL_H_
#define MIXTUREACOUSTICMODEL_H_

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

class TransValue {
  std::string state;
  float value;

 public:
  TransValue(const std::string &st, const float &val);
  std::string &getState() { return state; }
  float &getValue() { return value; }
};

TransValue::TransValue(const std::string &st, const float &val) {
  state = st;
  value = val;
}

class DGaussianState {
  std::vector<std::vector<float>> mus;
  std::vector<std::vector<float>> vars;
  std::vector<std::vector<float>> ivars;
  std::vector<float> pmembers;
  std::vector<float> logc;
  size_t components;

 public:
  // DGaussianState();
  void addPMembers(const std::string &line);
  void addMu(const std::string &line);
  void addVar(const std::string &line);
  void setComponents(const size_t comps) { components = comps; }
  size_t getComponents() { return components; }
  std::vector<std::vector<float>> &getMus() { return mus; }
  std::vector<float> &getMuByComponent(const size_t &component) {
    return mus[component];
  }
  std::vector<float> &getIVarByComponent(const size_t &component) {
    return ivars[component];
  }
  std::vector<std::vector<float>> &getVars() { return vars; }
  std::vector<float> &getPMembers() { return pmembers; }
  std::vector<float> &getLogcs() { return logc; }
};

class MixtureAcousticModel {
  typedef std::tuple<std::string, float> value_t;
  std::vector<std::string> states;
  std::unordered_map<std::string, std::vector<DGaussianState>> symbol_to_states;
  std::unordered_map<std::string, std::vector<float>> state_to_trans;
  std::unordered_map<std::string,
                     std::unordered_map<std::string, std::vector<TransValue>>>
      state_to_transL;
  std::unordered_map<std::string, std::string> state_to_type;
  std::unordered_map<std::string, int> state_to_num_q;
  std::vector<float> smooth;
  unsigned int dim;
  unsigned int n_states;

 public:
  MixtureAcousticModel();
  explicit MixtureAcousticModel(const std::string &filename);
  ~MixtureAcousticModel();

  unsigned int getDim();
  unsigned int getNStates();

  void read_model(const std::string &filename);
  void write_model(const std::string &filename);
  float calc_prob(const std::string &state, const int &q,
                  const std::vector<float> &frame);
};

#endif  // MIXTUREACOUSTICMODEL_H_
