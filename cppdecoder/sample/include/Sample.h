/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */
#ifndef SAMPLE_H_
#define SAMPLE_H_

#include <Frame.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Sample {
  std::vector<Frame> frames;
  size_t dim;
  size_t num_frames;

 public:
  Sample() : dim(0), num_frames(0) {}
  size_t getDim() { return dim; }
  size_t getNFrames() { return num_frames; }
  Frame &getFrame(size_t t) { return frames[t]; }
  void write_sample(const std::string &filename);
  void setDim(size_t dim) { this->dim = dim; }
  void setNFrames(size_t num_frames) { this->num_frames = num_frames; }
  void addFrame(const std::string &line);
  void addFrame(const std::vector<float> &features);
  void show_content();
};

#endif  // SAMPLE_H_
