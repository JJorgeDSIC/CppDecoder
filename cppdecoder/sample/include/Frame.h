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
 /**
  * @brief Construct a new Frame object
  * 
  * @param[in] values Vector of float values for this frame
  */
  explicit Frame(const std::vector<float>& values);
  /**
   * @brief Get the Features object
   * 
   * @return std::vector<float>& the vector representing the float values for this frame
   */
  std::vector<float>& getFeatures() { return features; }
  /**
   * @brief Get the dimension of this frame
   * 
   * @return uint32_t 
   */
  uint32_t getDim() const { return dim; }
  /**
   * @brief Shows the content of this frame using stdout
   * 
   */
  void show_content();

 private:
  std::vector<float> features;
  uint32_t dim;
};

#endif  // FRAME_H_
