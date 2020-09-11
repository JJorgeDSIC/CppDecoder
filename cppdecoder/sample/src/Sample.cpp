/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include "Sample.h"

Sample::Sample() : dim(0), num_frames(0) {}

Sample::Sample(size_t d, size_t n) : dim(d), num_frames(n) {
  frames.reserve(num_frames);
}

void Sample::show_content() {
  for (auto &frame : frames) {
    frame.show_content();
  }
}

int Sample::addFrame(const std::string &line) {
  std::vector<float> frameVector = read_vector<float>(line);
  return this->addFrame(frameVector);
}

int Sample::addFrame(const std::vector<float> &features) {
  if (dim != 0) {
    if (dim != features.size()) {
      std::cout << "Incorrect dimension: expected " << frames[0].getDim()
                << ", provided: " << features.size() << std::endl;
      return 1;
    }
  } else {
    dim = features.size();
  }
  frames.emplace_back(features);
  num_frames++;
  return 0;
}

int Sample::write_sample(const std::string &filename) {
  std::ofstream fileO(filename, std::ios::app);

  if (fileO.is_open()) {
    fileO << "AKREALTF" << std::endl;
    fileO << dim << " " << num_frames << std::endl;

    for (auto &frame : frames) {
      std::vector<float> fram = frame.getFeatures();
      for (size_t i = 0; i < fram.size() - 1; i++) {
        fileO << fram[i] << " ";
      }

      fileO << fram[fram.size() - 1] << std::endl;
    }
    fileO.close();

  } else {
    std::cout << "Unable to open file for writing" << std::endl;
    return 1;
  }
  return 0;
}

int Sample::read_sample(const std::string &filename) {
  std::ifstream fileI(filename, std::ifstream::in);

  std::string line;

  if (fileI.is_open()) {
    size_t dim, n_frames;
    getline(fileI, line);

    getline(fileI, line);
    std::stringstream ss(line);

    size_t temp_n_frames;

    ss >> dim;
    ss >> temp_n_frames;
    std::vector<float> values;

    frames.reserve(temp_n_frames);

    for (size_t i = 0; i < temp_n_frames; i++) {
      getline(fileI, line);
      values = read_vector<float>(line);
      addFrame(values);
    }

    if (temp_n_frames != num_frames) {
      std::cout << "number of frames differ" << std::endl;
      return 1;
    }

    fileI.close();
  } else {
    std::cout << "Unable to open the file " << filename << " for reading."
              << std::endl;
    return 1;
  }
  return 0;
}
