/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */
#ifndef SAMPLE_H_
#define SAMPLE_H_

#include <Frame.h>

#include <fstream>
#include <iostream>
#include <iterator>
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
  int read_sample(const std::string &filename);
  int write_sample(const std::string &filename);
  int addFrame(const std::string &line);
  int addFrame(const std::vector<float> &features);
  void setDim(size_t dim) { this->dim = dim; }
  void setNFrames(size_t num_frames) { this->num_frames = num_frames; }
  void show_content();

  inline std::vector<Frame>::iterator begin() noexcept {
    return frames.begin();
  }
  inline std::vector<Frame>::iterator end() noexcept { return frames.end(); }

  inline std::vector<Frame>::const_iterator cbegin() const noexcept {
    return frames.cbegin();
  }
  inline std::vector<Frame>::const_iterator cend() const noexcept {
    return frames.cend();
  }
};

#endif  // SAMPLE_H_
