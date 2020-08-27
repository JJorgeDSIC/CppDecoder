#include "Utils.h"

const float LOGEPS = -36.0437;

void print_vector(const std::vector<float> &vec) {
  for (auto it = vec.begin(); it != vec.end(); ++it) std::cout << *it << ' ';
  std::cout << std::endl;
}

void print_vector(const std::vector<std::string> &vec) {
  for (auto it = vec.begin(); it != vec.end(); ++it) std::cout << *it << ' ';
  std::cout << std::endl;
}

float robust_add(const std::vector<float> &pprobs, const float &max,
                 const size_t &components) {
  size_t n;
  float res = 0.0;
  for (n = 0; n < components; ++n) {
    float aux = pprobs[n] - max;
    if (aux >= LOGEPS) res += exp(aux);
  }

  return max + log(res);
}

std::vector<float> parse_line(const std::string &line) {
  std::istringstream stm(line);
  return {std::istream_iterator<float>(stm), std::istream_iterator<float>()};
}
