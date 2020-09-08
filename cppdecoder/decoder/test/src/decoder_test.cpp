/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include <Decoder.h>
#include <Utils.h>
#include <stdio.h>

#include <iomanip>  // std::setprecision

#include "gtest/gtest.h"

namespace {

// TODO: Create more test cases

class DecoderTests : public ::testing::Test {
 protected:
  const std::string nameModelDgaussian =
      "./models/dgaussian_monopohoneme_I01.example.model";
};

TEST_F(DecoderTests, DecoderConstructor) { ASSERT_TRUE(true); }

}  // namespace
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
