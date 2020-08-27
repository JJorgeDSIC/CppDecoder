#include "Feature.h"
#include <Utils.h>

Feature::Feature(const std::vector<float> &values) {
  std::cout << "Constructor " << std::endl;

  for (auto &value : values) {
    features.push_back(value);
  }
  dim = features.size();
}

void Feature::show_content() {
  for ( auto &feature : features ) {
    std::cout << feature << " ";
  }
  std::cout << std::endl;
}

int main() {
  std::cout << "Main feature..." << std::endl;

  std::vector<float> frame = {
      -0.392699, -2.06331,  0.0109949,  0.0630278, 0.713447,    -0.557419,
      1.46355,   0.809983,  0.990555,   0.682074,  -1.62765,    0.60225,
      0.493882,  1.55829,   -0.59736,   -1.34481,  -0.0268113,  -0.0561324,
      0.536304,  1.16865,   0.753556,   -0.813899, -0.370601,   -0.346987,
      -1.12761,  0.0755082, -1.127,     -1.23163,  0.717646,    -0.20932,
      0.515273,  0.0881923, 0.00711961, 0.294303,  -0.00440401, -0.600391,
      -0.627719, 0.292688,  0.360419,   -0.443323, -0.189734,   0.420539,
      0.881978,  0.19503,   -0.93659,   -0.414377, 0.544633,    0.00430982};

  Feature feas(frame);

  feas.show_content();

  std::cout << std::endl;

  std::cout << feas.getDim() << std::endl;
}
