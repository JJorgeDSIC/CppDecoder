/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef MIXTUREACOUSTICMODEL_H_
#define MIXTUREACOUSTICMODEL_H_

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "DGaussianAcousticModel.h"

class TransValue {
 public:
  TransValue(const std::string &st, const float val);

  std::string &getState() { return state; }

  float getValue() const { return value; }

 private:
  std::string state;
  float value;
};

class GaussianMixtureState {
 public:
  GaussianMixtureState();

  GaussianMixtureState(uint32_t components, uint32_t dim);

  uint32_t getComponents() const { return components; }

  void setComponents(const uint32_t comps) { components = comps; }

  void reserveComponents(const uint32_t comps);

  void addPMembers(const std::string &line);

  int addGaussianState(const uint32_t d, const std::string &mu_line,
                       const std::string &var_line);

  std::vector<float> &getMuByComponent(const uint32_t component) {
    return gstates[component].getMu();
  }
  std::vector<float> &getVarByComponent(const uint32_t component) {
    return gstates[component].getVar();
  }

  uint32_t getDim() const { return dim; }

  void setDim(const uint32_t dim) { this->dim = dim; }

  std::vector<float> &getPMembers() { return pmembers; }

  float calc_logprob(const std::vector<float> &frame);

 private:
  std::vector<GaussianState> gstates;
  std::vector<float> pmembers;
  uint32_t components;
  uint32_t dim;
};

class MixtureAcousticModel : public AcousticModel {
 public:
  explicit MixtureAcousticModel(const std::string &filename);

  uint32_t getDim() override { return dim; }

  void setDim(const uint32_t dim) override { this->dim = dim; }

  uint32_t getNStates() override { return n_states; }

  int read_model(const std::string &filename) override;

  int write_model(const std::string &filename) override;

  float calc_logprob(const std::string &state, int q,
                     const std::vector<float> &frame) override;

  std::string &getStateTransType(const std::string &state);

  std::vector<float> &getStateTrans(const std::string &state);

 private:
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
  uint32_t dim;
  uint32_t n_states;
};

#endif  // MIXTUREACOUSTICMODEL_H_
