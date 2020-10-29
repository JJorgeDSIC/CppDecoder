/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef TIEDSTATESACOUSTICMODEL_H_
#define TIEDSTATESACOUSTICMODEL_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "MixtureAcousticModel.h"

class TiedStatesAcousticModel : public AcousticModel {
 public:
  /**
   * @brief Construct a new Tied States Acoustic Model object
   *
   * @param[in] filename file location
   */
  explicit TiedStatesAcousticModel(const std::string &filename);
  /**
   * @brief Get vectors's dimension.
   *
   * @return uint32_t The dimension.
   */
  uint32_t getDim() override { return dim; }
  /**
   * @brief Get the number of states contained in this model.
   *
   * @return uint32_t The number of states.
   */
  uint32_t getNStates() override { return n_states; }

  /**
   * @brief Set vectors's dimension.
   *
   * @param[in] d The new dimension.
   */
  void setDim(const uint32_t d) override { dim = d; };
  /**
   * @brief Read a TiedState Acoustic model from text file.
   *
   * @param[in] filename File location.
   * @return int 0 if everything is OK, 1 if there was a problem.
   */
  int read_model(const std::string &filename) override;
  /**
   * @brief Write a TiedState Acoustic model to text file.
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
   * @brief TODO
   *
   * @param state
   * @return std::string&
   */
  std::string &getStateTransType(const std::string &state);

 private:
  uint32_t dim;
  uint32_t n_states;
  uint32_t n_trans;
  std::vector<float> smooth;

  std::vector<std::string> symbols;
  std::vector<std::string> senones;

  std::unordered_map<std::string, std::vector<std::string>> symbol_to_senone;
  std::unordered_map<std::string, GaussianMixtureState> senone_to_mixturestate;
  std::unordered_map<std::string, std::string> symbol_to_symbol_transitions;
  std::unordered_map<std::string, std::vector<float>> symbol_to_transitions;
  std::unordered_map<std::string, std::vector<std::string>> symbol_to_senones;
  std::unordered_map<std::string, std::string> symbol_to_type;
};

#endif  // TIEDSTATESACOUSTICMODEL_H_
