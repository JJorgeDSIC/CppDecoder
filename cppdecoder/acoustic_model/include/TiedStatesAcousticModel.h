/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef TIEDSTATESACOUSTICMODEL_H_
#define TIEDSTATESACOUSTICMODEL_H_

#include "MixtureAcousticModel.h"

class TiedStatesAcousticModel: public AcousticModel{
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
   /**
   * @brief Construct a new TiedStatesAcousticModel
   *
   * @param filename File location
   */
  explicit TiedStatesAcousticModel(const std::string &filename);
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
   * @brief Read a Tied State Acoustic model from text file
   *
   * @param filename File location
   * @return int 0 if everything was OK, 1 if there was a problem
   */
  int read_model(const std::string &filename) override;
    /**
   * @brief Write a Tied State Acoustic model to text file
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

#endif  // TIEDSTATESACOUSTICMODEL_H_
