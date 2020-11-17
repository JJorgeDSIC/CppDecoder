/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef DGAUSSIANACOUSTICMODEL_H_
#define DGAUSSIANACOUSTICMODEL_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "AcousticModel.h"

class GaussianState {
  std::vector<float> mu;
  std::vector<float> var;
  std::vector<float> ivar;
  float logc;
  uint32_t dim;

 public:
  /**
   * @brief Initialize a new Gaussian State object.
   *
   * @param[in] d Vectors' dimension.
   */
  explicit GaussianState(const uint32_t d);

  /**
   * @brief Construct a new Gaussian State object, providing mu and var line.
   *
   * @param[in] d Vector's dimension.
   * @param[in] mu_line Mu in text format (i.e: val1 val2 ... val_dim).
   * @param[in] var_line Var in text format (i.e: val1 val2 ... val_dim).
   */
  GaussianState(const uint32_t d, const std::string &mu_line,
                const std::string &var_line);

  /**
   * @brief Add Mu in string format.
   *
   * @param[in] line Line containing the values (i.e: val1 val2 ... val_dim).
   */
  void addMu(const std::string &line);

  /**
   * @brief Add Variance in string format.
   *
   * @param[in] line Line containing the values (i.e: val1 val2 ... val_dim).
   */
  void addVar(const std::string &line);

  /**
   * @brief Set Log_c value.
   *
   * @param[in] logc The new Log_c value.
   */
  void setLogc(float logc) { this->logc = logc; }

  /**
   * @brief Get state's mu.
   *
   * @return std::vector<float>& with mu values.
   */
  std::vector<float> &getMu() { return mu; }

  /**
   * @brief Get state's variance.
   *
   * @return std::vector<float>& with variance values.
   */
  std::vector<float> &getVar() { return var; }

  /**
   * @brief Get state's 1/variance.
   *
   * @return std::vector<float>&  with 1 / variance values.
   */
  std::vector<float> &getIVar() { return ivar; }

  /**
   * @brief Get vectors's dimension.
   *
   * @return uint32_t The dimension.
   */
  uint32_t getDim() const { return dim; }

  /**
   * @brief Get the Logc value.
   *
   * @return float The Logc value.
   */
  float getLogc() const { return logc; }

  /**
   * @brief Print state's values: mu, var and logc.
   */
  void print_state();

  /**
   * @brief Provides the log probability of the provided frame.
   *
   * @param[in] frame Frame to be used to compute the log probability with
   * state's parameters.
   * @return float log probability.
   */
  float calc_logprob(const std::vector<float> &frame);
};

class DGaussianAcousticModel : public AcousticModel {
  std::vector<std::string> states;
  std::unordered_map<std::string, std::vector<std::unique_ptr<GaussianState>>>
      state_to_gstate;
  std::unordered_map<std::string, std::vector<float>> state_to_trans;
  std::unordered_map<std::string, int> state_to_num_q;
  std::unordered_map<std::string, std::string> state_to_type;

  std::vector<float> smooth;
  uint32_t dim;
  uint32_t n_states;

 public:
  /**
   * @brief Construct a new DGaussianAcousticModel.
   *
   * @param[in] filename File location.
   */
  explicit DGaussianAcousticModel(const std::string &filename);
  /**
   * @brief Get vectors's dimension.
   *
   * @return uint32_t The dimension.
   */
  uint32_t getDim() const override { return dim; }

  /**
   * @brief Set vectors's dimension
   *
   * @param[in] d The new dimension.
   */
  void setDim(const uint32_t d) override { dim = d; };

  /**
   * @brief Get the number of states contained in this model.
   *
   * @return uint32_t The number os states.
   */
  uint32_t getNStates() const override { return n_states; }

  /**
   * @brief Read a Diagonal Gaussian Acoustic model from text file.
   *
   * @param[in] filename File location.
   * @return int 0 if everything is OK, 1 if there was a problem.
   */
  int read_model(const std::string &filename) override;

  /**
   * @brief Write a Diagonal Gaussian Acoustic model to text file.
   *
   * @param[in] filename File location.
   * @return int 0 if everything is OK, 1 if there was a problem.
   */
  int write_model(const std::string &filename) override;

  /**
   * @brief Provides the log probability for a frame F, being in a state S and
   * the state Q of the HMM.
   *
   * @param[in] state Acoustic Model state.
   * @param[in] q Hidden Markov Model state.
   * @param[in] frame Frame to use to compute the log probability.
   * @return float Log probability for a frame being in the state S and the HMM
   *               state Q.
   */
  float calc_logprob(const std::string &state, const int q,
                     const std::vector<float> &frame) override;

  /**
   * @brief Get the State Trans Type from symbol/state
   *
   * @param state
   * @return std::string&
   */
  std::string &getStateTransType(const std::string &state) override;

  /**
   * @brief Get the state transitions in vector form, containing for each position the loop's log prob
   *
   * @param state
   * @return std::vector<float>&
   */
  std::vector<float> &getStateTrans(const std::string &state) override;
};

#endif  // DGAUSSIANACOUSTICMODEL_H_
