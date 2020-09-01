/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

const float LOG2PI = 1.83787706641f;
const float LOGEPS = -36.0437;

template <typename T>
std::vector<T> read_vector(const std::string &line);

template <typename T>
void print_vector(const std::vector<T> &vec);

template <typename T>
void print_vector(const std::vector<T> &vec) {
  for (auto it = vec.begin(); it != vec.end(); ++it) std::cout << *it << ' ';
  std::cout << std::endl;
}

template <typename T>
std::vector<T> read_vector(const std::string &line) {
  std::istringstream stm(line);
  return {std::istream_iterator<T>(stm), std::istream_iterator<T>()};
}

float robust_add(const std::vector<float> &pprobs, const float &max,
                 const size_t &components);

size_t read_header_line(std::ifstream &fileI, std::string line, const char del);

std::vector<float> parse_line(const std::string &line);

#endif  // UTILS_H_
