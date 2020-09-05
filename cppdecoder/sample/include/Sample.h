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
  std::vector<Frame> frames;
  size_t dim;
  size_t num_frames;

 public:
 /**
  * @brief Construct a new Sample object
  * 
  */
  Sample() : dim(0), num_frames(0) {}
  /**
   * @brief Construct a new Sample object, specifying the dimension and the number of vectors
   * 
   * @param dim Vector dimension
   * @param num_frames Number of frames in the sample
   */
  Sample(size_t dim, size_t num_frames);
  /**
   * @brief Get the dimension of the vectors
   * 
   * @return size_t Vector dimension
   */
  size_t getDim() { return dim; }
  /**
   * @brief Get the number of frames in this sample
   * 
   * @return size_t Number of frames in the sample
   */
  size_t getNFrames() { return num_frames; }
  /**
   * @brief Get the Frame object at position index
   * 
   * @param index Index inside the sample, usually temporal dimension 
   * @return Frame& Frame object at position index in the sample
   */
  Frame &getFrame(size_t index) { return frames[index]; }
  /**
   * @brief Read a Sample from text file
   * 
   * @param filename File location
   * @return int 0 if everything is OK, 1 if there was a problem
   */
  int read_sample(const std::string &filename);
  /**
   * @brief Write a Sample into a text file
   * 
   * @param filename File location
   * @return int 0 if everything is OK, 1 if there was a problem
   */
  int write_sample(const std::string &filename);
  /**
   * @brief Add a frame in the sample from a string 
   * 
   * @param line String with the vector formar, i.e: val1 val2 ... valn \n
   * @return int 0 if everything is OK, 1 if there was a problem
   */
  int addFrame(const std::string &line);
  /**
   * @brief Add a frame from a vector of floats.
   * 
   * @param features Vector of floats containing the values for the frame, it should have the proper dimension.
   * @return int 0 if everything is OK, 1 if there was a problem
   */
  int addFrame(const std::vector<float> &features);
  /**
   * @brief Set the dimension for this sample
   * 
   * @param dim Vector dimension
   */
  void setDim(size_t dim) { this->dim = dim; }
  /**
   * @brief Set the number of frames for this sample
   * 
   * @param num_frames Number of frames
   */
  void setNFrames(size_t num_frames) { this->num_frames = num_frames; }
  /**
   * @brief Shows the content of this sample
   * 
   */
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
