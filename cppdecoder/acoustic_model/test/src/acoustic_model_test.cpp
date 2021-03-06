/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include <AcousticModel.h>
#include <DGaussianAcousticModel.h>
#include <MixtureAcousticModel.h>
#include <TiedStatesAcousticModel.h>
#include <Utils.h>
#include <stdio.h>

#include <iomanip>  // std::setprecision

#include "gtest/gtest.h"

namespace {

// TODO: Create more test cases

class AcousticModelTests : public ::testing::Test {
 protected:
  const std::string nameModelDgaussian =
      "./models/dgaussian_monopohoneme_I01.example.model";

  const std::string nameWrittenModelDgaussian =
      "./models/dgaussian_monopohoneme_I01.example.model.test";

  const std::string nameModelMixture =
      "./models/mixture_monophoneme_I32.example.model";

  const std::string nameWrittenModelMixture =
      "./models/mixture_monophoneme_I32.example.model.test";

  const std::string nameModelTiedStates =
      "./models/tiedphoneme_I04.example.model";

  const std::string nameWrittenModelTiedStates =
      "./models/tiedphoneme_I04.example.model.test";

  std::ifstream fileNameModel;
  std::ifstream fileNameWrittenModel;

  const std::vector<float> frame = {
      -0.392699, -2.06331,  0.0109949,  0.0630278, 0.713447,    -0.557419,
      1.46355,   0.809983,  0.990555,   0.682074,  -1.62765,    0.60225,
      0.493882,  1.55829,   -0.59736,   -1.34481,  -0.0268113,  -0.0561324,
      0.536304,  1.16865,   0.753556,   -0.813899, -0.370601,   -0.346987,
      -1.12761,  0.0755082, -1.127,     -1.23163,  0.717646,    -0.20932,
      0.515273,  0.0881923, 0.00711961, 0.294303,  -0.00440401, -0.600391,
      -0.627719, 0.292688,  0.360419,   -0.443323, -0.189734,   0.420539,
      0.881978,  0.19503,   -0.93659,   -0.414377, 0.544633,    0.00430982};

  const std::vector<float> wrongFrame = {
      -0.392699, -2.06331,  0.0109949,  0.0630278, 0.713447,    -0.557419,
      1.46355,   0.809983,  0.990555,   0.682074,  -1.62765,    0.60225,
      0.493882,  1.55829,   -0.59736,   -1.34481,  -0.0268113,  -0.0561324,
      0.536304,  1.16865,   0.753556,   -0.813899, -0.370601,   -0.346987,
      -1.12761,  0.0755082, -1.127,     -1.23163,  0.717646,    -0.20932,
      0.515273,  0.0881923, 0.00711961, 0.294303,  -0.00440401, -0.600391,
      -0.627719, 0.292688,  0.360419,   -0.443323, -0.189734,   0.420539};

  const std::string lineMu =
      "0.611003 -0.341059 -0.767879 -1.00645 0.271412 1.00703 0.143509 "
      "-0.565818 -0.0726448 -0.0827588 -0.0635558 -0.176374 -0.551339 "
      "-0.164538 -0.118646 0.588949 0.616664 -0.51543 -0.518155 -0.62594 "
      "0.0153023 0.314522 -0.276895 -0.245692 0.0431228 -0.0701371 -0.174652 "
      "-0.403049 -0.477148 -0.14131 -0.0339165 0.706629 -0.205301 -0.147137 "
      "0.0124271 0.272251 0.0275675 -0.258583 -0.120088 0.286936 0.138446 "
      "0.0438449 -0.0294147 -0.056371 0.137235 0.138078 0.129186 -0.0791308";
  const std::string lineVar =
      "0.431411 0.647725 0.612571 1.25852 0.904831 1.30368 1.15083 0.858501 "
      "0.854562 1.14167 1.05908 1.02635 1.15654 1.08413 1.06181 0.290333 "
      "1.34178 1.03543 1.03234 1.53382 1.11535 1.3774 1.25368 1.23262 1.39954 "
      "1.37645 1.24627 1.31598 1.23377 1.21376 1.20752 0.732586 0.735857 "
      "0.853519 0.998284 1.20589 0.952269 1.19117 0.905265 1.01046 0.928685 "
      "1.08421 1.13455 1.03278 1.09966 1.08389 1.02634 0.599768";
};

TEST_F(AcousticModelTests, AcousticModelReadWrite) {
  DGaussianAcousticModel acousticmodel(nameModelDgaussian);

  std::vector<AcousticModel*> amodels;

  amodels.push_back(&acousticmodel);

  amodels[0]->write_model(nameWrittenModelDgaussian);
  fileNameModel.open(nameModelDgaussian);
  fileNameWrittenModel.open(nameWrittenModelDgaussian);

  std::string lineA;
  std::string lineB;

  while (getline(fileNameModel, lineA) &&
         getline(fileNameWrittenModel, lineB)) {
    if (lineA != lineB) {
      fileNameWrittenModel.close();
      fileNameModel.close();
      if (remove(nameWrittenModelDgaussian.c_str()) != 0)
        std::cout << "Error deleting file" << std::endl;
      else
        std::cout << "File successfully deleted" << std::endl;
    }
    ASSERT_EQ(lineA, lineB);
  }

  fileNameWrittenModel.close();
  fileNameModel.close();
  if (remove(nameWrittenModelDgaussian.c_str()) != 0)
    std::cout << "Error deleting file" << std::endl;
  else
    std::cout << "File successfully deleted" << std::endl;
  ASSERT_TRUE(true);
}

TEST_F(AcousticModelTests, AcousticModelTestsCalcProbWrongStateDGaussian) {
  DGaussianAcousticModel diagonalAcousticModel(nameModelDgaussian);

  std::vector<AcousticModel*> amodels;

  amodels.push_back(&diagonalAcousticModel);

  float prob = amodels[0]->calc_logprob("aaaaaaaaaa", 0, frame);

  ASSERT_FLOAT_EQ(prob, INFINITY);
}

TEST_F(AcousticModelTests, AcousticModelTestsCalcProbStateDGaussian) {
  DGaussianAcousticModel diagonalAcousticModel(nameModelDgaussian);

  std::vector<AcousticModel*> amodels;

  amodels.push_back(&diagonalAcousticModel);

  float prob = amodels[0]->calc_logprob("aa", 0, frame);

  float probTrue = -69.51816177368164;

  ASSERT_FLOAT_EQ(prob, probTrue);
}

TEST_F(AcousticModelTests, AcousticModelTestsCalcProbStateMixtureModel) {
  MixtureAcousticModel mixAcousticModel(nameModelMixture);

  std::vector<AcousticModel*> amodels;

  amodels.push_back(&mixAcousticModel);

  float prob = amodels[0]->calc_logprob("a", 0, frame);

  float probTrue = -51.783161;  // To review
  ASSERT_FLOAT_EQ(prob, probTrue);
}

TEST_F(AcousticModelTests, AcousticModelTestsCalcProbWrongStateMixtureModel) {
  MixtureAcousticModel mixAcousticModel(nameModelMixture);

  std::vector<AcousticModel*> amodels;

  amodels.push_back(&mixAcousticModel);

  float prob = amodels[0]->calc_logprob("aaaaaaaaaa", 0, frame);
  ASSERT_FLOAT_EQ(prob, INFINITY);
}


TEST_F(AcousticModelTests, AcousticModelTestsCalcProStateTiedState) {
  TiedStatesAcousticModel tiedstatesacousticmodel(nameModelTiedStates);

  std::vector<AcousticModel*> amodels;

  amodels.push_back(&tiedstatesacousticmodel);

  float prob = amodels[0]->calc_logprob("aa_B+l_E", 0, frame);

  float probTrue = -56.440113;  // To review
  ASSERT_FLOAT_EQ(prob, probTrue);
}


TEST_F(AcousticModelTests, AcousticModelTestsCalcProbWrongStateTiedState) {
  TiedStatesAcousticModel tiedstatesacousticmodel(nameModelTiedStates);

  std::vector<AcousticModel*> amodels;

  amodels.push_back(&tiedstatesacousticmodel);

  float prob = amodels[0]->calc_logprob("aaaaaaaaaa", 0, frame);
  ASSERT_FLOAT_EQ(prob, INFINITY);
}

}  // namespace
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
