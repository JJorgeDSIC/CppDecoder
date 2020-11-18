/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include "Frame.h"

Frame::Frame(const std::vector<float> &values)
    : features(values.begin(), values.end()), dim(values.size()) {}

void Frame::show_content() {
  for (const auto &feature : features) {
    std::cout << feature << " ";
  }
  std::cout << std::endl;
}
