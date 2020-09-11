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
  explicit TiedStatesAcousticModel(const std::string &filename);

  uint32_t getDim() override { return dim; }

  uint32_t getNStates() override { return n_states; }

  /**
   * @brief Set vectors's dimension.
   *
   * @param[in] d The new dimension.
   */
  void setDim(const uint32_t d) override { dim = d; };

  int read_model(const std::string &filename) override;

  int write_model(const std::string &filename) override;

  float calc_logprob(const std::string &state, const int q,
                     const std::vector<float> &frame) override;

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
};

#endif  // TIEDSTATESACOUSTICMODEL_H_
