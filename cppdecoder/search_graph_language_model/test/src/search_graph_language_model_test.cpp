/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#include <SearchGraphLanguageModel.h>
#include <Utils.h>
#include <stdio.h>

#include <iomanip>  // std::setprecision

#include "gtest/gtest.h"

namespace {


class SearchGraphLanguageModelTests : public ::testing::Test {
 protected:
  const std::string nameModel =
      "TO DO";

};

TEST_F(SearchGraphLanguageModelTests, SearchGraphLanguageModelTest) { ASSERT_TRUE(true); }

}  // namespace
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
