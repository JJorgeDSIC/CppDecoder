/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: BSD 2-Clause License
 */

#include <Utils.h>

#include <vector>

#include "gtest/gtest.h"

TEST(Utils, RobustAddTest) {
  std::vector<float> values = {
      0.611003,   -0.341059,  -0.767879,  -1.00645,   0.271412,   1.00703,
      0.143509,   -0.565818,  -0.0726448, -0.0827588, -0.0635558, -0.176374,
      -0.551339,  -0.164538,  -0.118646,  0.588949,   0.616664,   -0.51543,
      -0.518155,  -0.62594,   0.0153023,  0.314522,   -0.276895,  -0.245692,
      0.0431228,  -0.0701371, -0.174652,  -0.403049,  -0.477148,  -0.14131,
      -0.0339165, 0.706629,   -0.205301,  -0.147137,  0.0124271,  0.272251,
      0.0275675,  -0.258583,  -0.120088,  0.286936,   0.138446,   0.0438449,
      -0.0294147, -0.056371,  0.137235,   0.138078,   0.129186,   -0.0791308};

  float max = 1.00703;

  size_t comps = 48;

  float resutlTrue = 3.8889095518008068;

  ASSERT_FLOAT_EQ(robust_add(values, max, comps),
            resutlTrue);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
