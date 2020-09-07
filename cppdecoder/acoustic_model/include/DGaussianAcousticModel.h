/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef DGAUSSIANACOUSTICMODEL_H_
#define DGAUSSIANACOUSTICMODEL_H_

#include "AcousticModel.h"

class GaussianState {
  std::vector<float> mu;
  std::vector<float> var;
  std::vector<float> ivar;
  float logc;
  size_t dim;

 public:
  /**
   * @brief Initialize a new Gaussian State object
   *
   * @param dim Vectors' dimension
   */
  explicit GaussianState(size_t dim) {
    this->dim = dim;
    mu.reserve(dim);
    var.reserve(dim);
    ivar.reserve(dim);
    logc = 0;
  }
  /**
   * @brief Construct a new Gaussian State object, providing mu and var line
   *
   * @param dim Vector's dimension
   * @param mu_line Mu in text format (i.e: val1 val2 ... val_dim)
   * @param var_line Var in text format (i.e: val1 val2 ... val_dim)
   */
  explicit GaussianState(size_t dim, const std::string &mu_line,
                         const std::string &var_line) {
    this->dim = dim;
    mu.reserve(dim);
    var.reserve(dim);
    ivar.reserve(dim);
    logc = 0;
    addMu(mu_line);
    addVar(var_line);
  }
  /**
   * @brief Add Mu in string format
   *
   * @param line Line containing the values (i.e: val1 val2 ... val_dim)
   */
  void addMu(const std::string &line);
  /**
   * @brief Add Variance in string format
   *
   * @param line Line containing the values (i.e: val1 val2 ... val_dim)
   */
  void addVar(const std::string &line);
  /**
   * @brief Set Log_c value
   *
   * @param logc
   */
  void setLogc(float logc) { this->logc = logc; }
  /**
   * @brief Get state's mu
   *
   * @return std::vector<float>& with mu values
   */
  std::vector<float> &getMu() { return mu; }
  /**
   * @brief Get state's variance
   *
   * @return std::vector<float>& with variance values
   */
  std::vector<float> &getVar() { return var; }
  /**
   * @brief Get state's 1/variance
   *
   * @return std::vector<float>&  with 1 / variance values
   */
  std::vector<float> &getIVar() { return ivar; }
  /**
   * @brief Get vectors's dimension
   *
   * @return size_t
   */
  size_t getDim() const { return dim; }
  /**
   * @brief Get the Logc value
   *
   * @return float
   */
  float getLogc() const { return logc; }
  /**
   * @brief Print state's values: mu, var and logc
   *
   */
  void print_state();
  /**
   * @brief Provides the log probability of the provided frame
   *
   * @param frame Frame to be used to compute the log probability with state's
   * parameters
   * @return float log probability
   */
  float calc_logprob(const std::vector<float> &frame);
};

class DGaussianAcousticModel: public AcousticModel {
  std::vector<std::string> states;
  std::unordered_map<std::string, std::vector<GaussianState>> state_to_gstate;
  std::unordered_map<std::string, std::vector<float>> state_to_trans;
  std::unordered_map<std::string, int> state_to_num_q;
  std::vector<float> smooth;
  size_t dim;
  size_t n_states;

 public:
  /**
   * @brief Construct a new DGaussianAcousticModel
   *
   * @param filename File location
   */
  explicit DGaussianAcousticModel(const std::string &filename);
  /**
   * @brief Get vectors's dimension
   *
   * @return size_t
   */
  size_t getDim() const { return dim; }
  /**
   * @brief Get the number of states contained in this model
   *
   * @return size_t
   */
  size_t getNStates() const { return n_states; }
  /**
   * @brief Read a Diagonal Gaussian Acoustic model from text file
   *
   * @param filename File location
   * @return int 0 if everything is OK, 1 if there was a problem
   */
  int read_model(const std::string &filename);
  /**
   * @brief Write a Diagonal Gaussian Acoustic model to text file
   *
   * @param filename File location
   * @return int 0 if everything is OK, 1 if there was a problem
   */
  int write_model(const std::string &filename);
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
                     const std::vector<float> &frame);
};

#endif  // DGAUSSIANACOUSTICMODEL_H_
