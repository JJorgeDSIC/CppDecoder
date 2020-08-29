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

int main() {
  std::vector<float> frame = {
      -0.392699, -2.06331,  0.0109949,  0.0630278, 0.713447,    -0.557419,
      1.46355,   0.809983,  0.990555,   0.682074,  -1.62765,    0.60225,
      0.493882,  1.55829,   -0.59736,   -1.34481,  -0.0268113,  -0.0561324,
      0.536304,  1.16865,   0.753556,   -0.813899, -0.370601,   -0.346987,
      -1.12761,  0.0755082, -1.127,     -1.23163,  0.717646,    -0.20932,
      0.515273,  0.0881923, 0.00711961, 0.294303,  -0.00440401, -0.600391,
      -0.627719, 0.292688,  0.360419,   -0.443323, -0.189734,   0.420539,
      0.881978,  0.19503,   -0.93659,   -0.414377, 0.544633,    0.00430982};

  Frame feas(frame);

  feas.show_content();

  std::cout << std::endl;

  std::cout << feas.getDim() << std::endl;

  int n_frames = 100;
  Sample sample;
  for (size_t i = 0; i < n_frames; i++) {
    sample.addFrame(frame);
  }

  sample.show_content();

  std::cout << "Sample contains " << sample.getNFrames()
            << " frames with dim: " << sample.getDim() << std::endl;

  Sample sampleFromTLFEA;

  std::string filename = "bin/samples/AAFA0016.features";
  std::ifstream fileI(filename, std::ifstream::in);

  std::string line;
  if (fileI.is_open()) {
    size_t dim;

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
      sampleFromTLFEA.addFrame(values);
    }

    fileI.close();
  } else {
    std::cout << "Unable to open the file " << filename << " for reading."
              << std::endl;
  }

  std::cout << "Testing iterator implementation..." << std::endl;

  for (auto &value : sampleFromTLFEA) {
    value.show_content();
  }

  sampleFromTLFEA.write_sample("bin/samples/example.features");

  std::cout << "Testing reading after writing..." << std::endl;

  sampleFromTLFEA.read_sample("bin/samples/example.features");
}
