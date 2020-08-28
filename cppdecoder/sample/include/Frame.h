/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */
#ifndef FRAME_H_
#define FRAME_H_

#include <iostream>
#include <vector>

class Frame {
  std::vector<float> features;
  size_t dim;

 public:
  explicit Frame(const std::vector<float> &values);
  size_t getDim() { return dim; }
  void show_content();
};

#endif  // FRAME_H_
