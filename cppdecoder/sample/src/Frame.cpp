#include "Frame.h"

Frame::Frame(const std::vector<float> &values) {
  for (auto &value : values) {
    features.push_back(value);
  }
  dim = features.size();
}

void Frame::show_content() {
  for ( auto &feature : features ) {
    std::cout << feature << " ";
  }
  std::cout << std::endl;
}
