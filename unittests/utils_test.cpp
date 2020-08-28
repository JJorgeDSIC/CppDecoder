/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: BSD 2-Clause License
 */

#include <Utils.h>

#include <vector>

#include "gtest/gtest.h"

TEST(Utils, RobustAddTest) {
  ASSERT_EQ(robust_add(std::vector<float>(10, 0), 0.0f, 10),
            robust_add(std::vector<float>(10, 1), 0.0f, 10));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
