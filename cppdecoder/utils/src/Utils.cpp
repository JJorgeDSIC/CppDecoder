/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include "Utils.h"

float robust_add(const std::vector<float> &pprobs, const float &max,
                 const uint32_t &components) {
  if (max == -HUGE_VAL) return -HUGE_VAL;

  uint32_t n;
  float res = 0.0;
  for (n = 0; n < components; ++n) {
    float aux = pprobs[n] - max;
    if (aux >= LOGEPS) res += exp(aux);
  }

  return max + log(res);
}

uint32_t read_header_line(std::ifstream &fileI, std::string line,
                        const char del) {
  uint32_t value;
  getline(fileI, line, del);  // PARAM
  getline(fileI, line);       // values
  std::stringstream(line) >> value;
  return value;
}
