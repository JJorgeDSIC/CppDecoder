/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */
#include <iostream>
#include <vector>

class Feature {
  std::vector<float> features;
  size_t dim;

 public:
  explicit Feature(const std::vector<float> &values);
  size_t getDim() { return dim; }
  void show_content();
};
