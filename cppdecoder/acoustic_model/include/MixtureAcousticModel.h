/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef MIXTUREACOUSTICMODEL_H_
#define MIXTUREACOUSTICMODEL_H_

#include "DGaussianAcousticModel.h"

class TransValue {
  std::string state;
  float value;

 public:
  TransValue(const std::string &st, float val) : state(st), value(val) {}
  std::string &getState() { return state; }
  float getValue() const { return value; }
};

class GaussianMixtureState {
  std::vector<GaussianState> gstates;
  std::vector<float> pmembers;
  size_t components;
  size_t dim;

 public:
  GaussianMixtureState() : components(0) {}
  explicit GaussianMixtureState(size_t components, size_t dim) {
    this->dim = dim;
    this->components = components;
    gstates.reserve(components);
  }
  size_t getComponents() const { return components; }
  void setComponents(size_t comps) { components = comps; }
  void reserveComponents(size_t comps) {
    this->components = comps;
    gstates.reserve(components);
    pmembers.reserve(components);
  }

  void addPMembers(const std::string &line);
  int addGaussianState(size_t dim, const std::string &mu_line,
                       const std::string &var_line);
  GaussianState &getGaussianStateByComponent(size_t component) {
    return gstates[component];
  }
  const std::vector<GaussianState> &getGStates() { return gstates; }
  size_t getDim() const { return dim; }
  void setDim(size_t dim) { this->dim = dim; }
  std::vector<float> &getPMembers() { return pmembers; }
  float calc_logprob(const std::vector<float> &frame);
};

class MixtureAcousticModel : public AcousticModel {
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
  /**
   * @brief Construct a new MixtureAcousticModel
   *
   * @param filename File location
   */
  explicit MixtureAcousticModel(const std::string &filename);
  /**
   * @brief Get vectors's dimension
   *
   * @return size_t
   */
  size_t getDim() override { return dim; }
  /**
   * @brief Get the number of states contained in this model
   *
   * @return size_t
   */
  size_t getNStates() override { return n_states; }
  /**
   * @brief Read a Mixture Gaussian Acoustic model from text file
   *
   * @param filename File location
   * @return int 0 if everything was OK, 1 if there was a problem
   */
  int read_model(const std::string &filename) override;
  /**
   * @brief Write a Mixture Gaussian Acoustic model to text file
   *
   * @param filename File location
   * @return int 0 if everything was OK, 1 if there was a problem
   */
  int write_model(const std::string &filename) override;
    /**
   * @brief Provides the log probability for a frame F, being in a state S and
   * the state Q of the HMM
   *
   * @param state Acoustic Model state
   * @param q Hidden Markov Model state
   * @param frame Frame to use to compute the log probability
   * @return float Log probability for a frame being in the state S and the HMM
   * state Q
   */
  float calc_logprob(const std::string &state, int q,
                     const std::vector<float> &frame) override;
};

#endif  // MIXTUREACOUSTICMODEL_H_
