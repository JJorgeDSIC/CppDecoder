#include "Utils.h"

const float LOGEPS = -36.0437;

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

size_t read_header_line(std::ifstream &fileI, std::string line,
                        const char del) {
  size_t value;
  getline(fileI, line, del);  // PARAM
  getline(fileI, line);       // values
  std::stringstream(line) >> value;
  return value;
}
