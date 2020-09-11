/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include <Sample.h>
#include <Utils.h>
#include <stdio.h>

#include <iomanip>  // std::setprecision

#include "gtest/gtest.h"

namespace {

class SampleTests : public ::testing::Test {
 protected:
  const std::string sampleFile = "./samples/AAFA0016.features";

  const std::string sampleFileWritten = "./samples/AAFA0016.features.test";

  std::ifstream fileStreamSample;
  std::ifstream fileStreamWrittenSample;

  const std::vector<float> frame = {
      -0.392699, -2.06331,  0.0109949,  0.0630278, 0.713447,    -0.557419,
      1.46355,   0.809983,  0.990555,   0.682074,  -1.62765,    0.60225,
      0.493882,  1.55829,   -0.59736,   -1.34481,  -0.0268113,  -0.0561324,
      0.536304,  1.16865,   0.753556,   -0.813899, -0.370601,   -0.346987,
      -1.12761,  0.0755082, -1.127,     -1.23163,  0.717646,    -0.20932,
      0.515273,  0.0881923, 0.00711961, 0.294303,  -0.00440401, -0.600391,
      -0.627719, 0.292688,  0.360419,   -0.443323, -0.189734,   0.420539,
      0.881978,  0.19503,   -0.93659,   -0.414377, 0.544633,    0.00430982};
};

TEST_F(SampleTests, SampleConstructionTest) {
  uint32_t dim = frame.size();
  uint32_t num_frames = 1;
  Sample sample(dim, num_frames);

  sample.addFrame(frame);

  ASSERT_EQ(sample.getFrame(0).getFeatures(), frame);
}

TEST_F(SampleTests, SampleReadWriteTest) {
  Sample sample;

  sample.read_sample(sampleFile);

  sample.write_sample(sampleFileWritten);

  fileStreamSample.open(sampleFile);
  fileStreamWrittenSample.open(sampleFileWritten);

  std::string lineA;
  std::string lineB;

  while (getline(fileStreamSample, lineA) &&
         getline(fileStreamWrittenSample, lineB)) {
    if (lineA != lineB) {
      fileStreamWrittenSample.close();
      fileStreamSample.close();
      if (remove(sampleFileWritten.c_str()) != 0)
        std::cout << "Error deleting file" << std::endl;
      else
        std::cout << "File successfully deleted" << std::endl;
    }
    ASSERT_EQ(lineA, lineB);
  }

  fileStreamWrittenSample.close();
  fileStreamSample.close();
  if (remove(sampleFileWritten.c_str()) != 0)
    std::cout << "Error deleting file" << std::endl;
  else
    std::cout << "File successfully deleted" << std::endl;
  ASSERT_TRUE(true);
}

}  // namespace
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
