/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef ACOUSTICMODEL_H_
#define ACOUSTICMODEL_H_

#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Utils.h"

class AcousticModel {
 public:
  /**
   * @brief Get vectors's dimension
   *
   * @return size_t
   */
  virtual size_t getDim();
  /**
   * @brief Get the number of states contained in this model
   *
   * @return size_t
   */
  virtual size_t getNStates();
  /**
   * @brief Read a Diagonal Gaussian Acoustic model from text file
   *
   * @param filename File location
   * @return int 0 if everything is OK, 1 if there was a problem
   */
  virtual int read_model(const std::string &filename);
  /**
   * @brief Write a Diagonal Gaussian Acoustic model to text file
   *
   * @param filename File location
   * @return int 0 if everything is OK, 1 if there was a problem
   */
  virtual int write_model(const std::string &filename);
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
  virtual float calc_logprob(const std::string &state, int q,
                             const std::vector<float> &frame);
};

#endif  // ACOUSTICMODEL_H_
