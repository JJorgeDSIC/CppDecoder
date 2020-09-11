/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef SAMPLE_H_
#define SAMPLE_H_

#include <Frame.h>
#include <Utils.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

class Sample {
 public:
  /**
   * @brief Construct a new Sample object
   */
  Sample();

  /**
   * @brief Construct a new Sample object, specifying the dimension and the
   *        number of vectors.
   *
   * @param[in] d Vector dimension.
   * @param[in] n Number of frames in the sample.
   */
  Sample(const size_t d, const size_t n);

  /**
   * @brief Get the dimension of the vectors.
   *
   * @return size_t Vector dimension.
   */
  size_t getDim() const { return dim; }

  /**
   * @brief Get the number of frames in this sample.
   *
   * @return size_t Number of frames in the sample.
   */
  size_t getNFrames() const { return num_frames; }

  /**
   * @brief Get the Frame object at position index.
   *
   * @param[in] index Index inside the sample, usually temporal dimension.
   * @return Frame& Frame object at position index in the sample.
   */
  Frame &getFrame(const size_t index) { return frames[index]; }

  /**
   * @brief Read a Sample from text file.
   *
   * @param[in] filename File location.
   * @return int 0 if everything is OK, 1 if there was a problem.
   */
  int read_sample(const std::string &filename);

  /**
   * @brief Write a Sample into a text file.
   *
   * @param[in] filename File location.
   * @return int 0 if everything is OK, 1 if there was a problem.
   */
  int write_sample(const std::string &filename);

  /**
   * @brief Add a frame in the sample from a string
   *
   * @param[in] line String with the vector formar, i.e: val1 val2 ... valn.
   * @return int 0 if everything is OK, 1 if there was a problem.
   */
  int addFrame(const std::string &line);

  /**
   * @brief Add a frame from a vector of floats.
   *
   * @param[in] features Vector of floats containing the values for the frame,
   * it should have the proper dimension.
   * @return int 0 if everything is OK, 1 if there was a problem.
   */
  int addFrame(const std::vector<float> &features);

  /**
   * @brief Set the dimension for this sample.
   *
   * @param[in] d Vector dimension.
   */
  void setDim(const size_t d) { dim = d; }

  /**
   * @brief Set the number of frames for this sample.
   *
   * @param[in] n Number of frames.
   */
  void setNFrames(const size_t n) { num_frames = n; }

  /**
   * @brief Shows the content of this sample
   */
  void show_content();

  std::vector<Frame>::iterator begin() noexcept;
  std::vector<Frame>::iterator end() noexcept;

  std::vector<Frame>::const_iterator cbegin() const noexcept;
  std::vector<Frame>::const_iterator cend() const noexcept;

 private:
  std::vector<Frame> frames;
  size_t dim;
  size_t num_frames;
};

#include "Sample.inl"

#endif  // SAMPLE_H_
