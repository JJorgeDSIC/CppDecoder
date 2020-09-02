/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include "Sample.h"

#include <Utils.h>

void Sample::show_content() {
  for (auto &frame : frames) {
    frame.show_content();
  }
}

int Sample::addFrame(const std::string &line) {
  std::cout << "TO DO" << std::endl;
  return 0;
}

int Sample::addFrame(const std::vector<float> &features) {
  if (dim != 0) {
    if (dim != features.size()) {
      std::cout << "Incorrect dimension: expected " << frames[0].getDim()
                << ", provided: " << features.size() << std::endl;
      return 0;
    }
  } else {
    dim = features.size();
  }

  Frame frame(features);
  frames.push_back(frame);
  num_frames++;
  return 1;
}

int Sample::write_sample(const std::string &filename) {
  std::ofstream fileO(filename, std::ios::app);

  if (fileO.is_open()) {
    fileO << "AKREALTF" << std::endl;
    fileO << dim << " " << num_frames << std::endl;
    for (auto &frame : frames) {
      for (auto &feature : frame.getFeatures()) {
        fileO << feature << " ";
      }
      fileO << std::endl;
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
    std::cout << line << std::endl;  // AKREALTF

    getline(fileI, line);
    std::stringstream ss(line);

    ss >> dim;
    std::cout << "DIM " << dim << std::endl;  // DIM
    ss >> n_frames;
    std::cout << "NFRAMES " << n_frames << std::endl;  // NFRAMES

    std::vector<float> values;

    for (size_t i = 0; i < n_frames; i++) {
      getline(fileI, line);
      std::cout << line << std::endl;  // VALUES
      values = parse_line(line);
      addFrame(values);
    }

    fileI.close();
  } else {
    std::cout << "Unable to open the file " << filename << " for reading."
              << std::endl;
    return 1;
  }
  return 0;
}
