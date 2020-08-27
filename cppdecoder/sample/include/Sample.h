/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */
#ifndef SAMPLE_H_
#define SAMPLE_H_

#include <Frame.h>

#include <iostream>
#include <string>
#include <vector>

class Sample {
  std::vector<Frame> frames;
  size_t dim;
  size_t num_frames;

 public:
  Sample(size_t d, size_t nframes) : dim(d), num_frames(nframes) {}
  size_t getDim() { return dim; }
  void addFrame(const std::string &line);
  void addFrame(const std::vector<float> &features);
  void show_content();
};

#endif  // SAMPLE_H_
