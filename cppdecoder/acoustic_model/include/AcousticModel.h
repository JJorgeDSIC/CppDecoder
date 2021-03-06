/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef ACOUSTICMODEL_H_
#define ACOUSTICMODEL_H_

#include <Utils.h>

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

class AcousticModel {
 public:
  virtual ~AcousticModel() {}

  /**
   * @brief Get vectors's dimension.
   *
   * @return uint32_t The dimension.
   */
  virtual uint32_t getDim() const = 0;

  /**
   * @brief Set vectors's dimension.
   *
   * @param[in] d The new dimension.
   */
  virtual void setDim(const uint32_t d) = 0;

  /**
   * @brief Get the number of states contained in this model.
   *
   * @return uint32_t The number of states.
   */
  virtual uint32_t getNStates() const = 0;

  /**
   * @brief Read a Diagonal Gaussian Acoustic model from text file.
   *
   * @param[in] filename File location.
   * @return int 0 if everything is OK, 1 if there was a problem.
   */
  virtual int read_model(const std::string &filename) = 0;

  /**
   * @brief Write a Diagonal Gaussian Acoustic model to text file.
   *
   * @param[in] filename File location.
   * @return int 0 if everything is OK, 1 if there was a problem.
   */
  virtual int write_model(const std::string &filename) = 0;

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
  virtual float calc_logprob(const std::string &state, const int q,
                             const std::vector<float> &frame) = 0;

  /**
   * @brief Get the State Trans Type from symbol/state
   *
   * @param state
   * @return std::string&
   */
  virtual std::string &getStateTransType(const std::string &state) = 0;

  /**
   * @brief Get the state transitions in vector form, containing for each position the loop's log prob
   *
   * @param state
   * @return std::vector<float>
   */
  virtual std::vector<float> &getStateTrans(const std::string &state) = 0;
};

#endif  // ACOUSTICMODEL_H_
