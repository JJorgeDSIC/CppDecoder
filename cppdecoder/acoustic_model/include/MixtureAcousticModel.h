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
#include <tuple>
#include <unordered_map>
#include <vector>

#include "DGaussianAcousticModel.h"
#include "Utils.h"

class TransValue {
  std::string state;
  float value;

 public:
  TransValue(const std::string &st, float val) : state(st), value(val) {}
  std::string &getState() { return state; }
  float &getValue() { return value; }
};

class GaussianMixtureState {
  std::vector<GaussianState> gstates;
  std::vector<float> pmembers;
  size_t components;

 public:
  GaussianMixtureState() : components(0) {}
  size_t getComponents() { return components; }
  void setComponents(size_t comps) { components = comps; }

  void addPMembers(const std::string &line);
  void addGaussianState(const GaussianState &state);
  GaussianState &getGaussianStateByComponent(size_t component) {
    return gstates[component];
  }
  std::vector<float> &getPMembers() { return pmembers; }
};

class MixtureAcousticModel {
  typedef std::tuple<std::string, float> value_t;
  std::vector<std::string> states;
  std::unordered_map<std::string, std::vector<GaussianMixtureState>>
      symbol_to_states;
  std::unordered_map<std::string, std::vector<float>> state_to_trans;
  std::unordered_map<std::string,
                     std::unordered_map<std::string, std::vector<TransValue>>>
      state_to_transL;
  std::unordered_map<std::string, std::string> state_to_type;
  std::unordered_map<std::string, int> state_to_num_q;
  std::vector<float> smooth;
  size_t dim;
  size_t n_states;

 public:
  explicit MixtureAcousticModel(const std::string &filename);
  size_t getDim();
  size_t getNStates();
  int read_model(const std::string &filename);
  int write_model(const std::string &filename);
  float calc_prob(const std::string &state, int q,
                  const std::vector<float> &frame);
};

#endif  // MIXTUREACOUSTICMODEL_H_
