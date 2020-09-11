/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <iostream>
#include <vector>

class Frame {
 public:
  explicit Frame(const std::vector<float>& values);
  std::vector<float>& getFeatures() { return features; }
  uint32_t getDim() const { return dim; }
  void show_content();

 private:
  std::vector<float> features;
  uint32_t dim;
};

#endif  // FRAME_H_
