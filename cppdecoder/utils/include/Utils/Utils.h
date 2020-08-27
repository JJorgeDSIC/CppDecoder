/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <cmath>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

void print_vector(const std::vector<float> &vec);

void print_vector(const std::vector<std::string> &vec);

float robust_add(const std::vector<float> &pprobs, const float &max,
                 const size_t &components);

std::vector<float> parse_line(const std::string &line);

#endif  // UTILS_H_
