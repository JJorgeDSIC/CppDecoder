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
  /**
   * @brief Construct a new Frame object
   *
   * @param values
   */
  explicit Frame(const std::vector<float>& values);
  /**
   * @brief Get the Features object, a vector with the actual values for this
   * frame
   *
   * @return std::vector<float>& Vector of floats with the values of this frame
   */
  std::vector<float>& getFeatures() { return features; }
  /**
   * @brief Get vectors's dimension
   *
   * @return size_t
   */
  size_t getDim() { return dim; }
  /**
   * @brief Show the content of this feature
   * 
   */
  void show_content();
};

#endif  // FRAME_H_
