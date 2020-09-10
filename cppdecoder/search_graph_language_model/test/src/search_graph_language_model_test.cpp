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
  const std::string SearchGraphFile = "./models/2.gram.graph";
  const std::string SearchGraphFileWritten = "./models/2.gram.graph.test";

  std::ifstream fileStreamSearchGraph;
  std::ifstream fileStreamWrittenSearchGraph;
};

TEST_F(SearchGraphLanguageModelTests, SearchGraphLanguageModelReadWriteTest) {
  SearchGraphLanguageModel sgraph;
  sgraph.read_model(SearchGraphFile);
  sgraph.write_model(SearchGraphFileWritten);

  fileStreamSearchGraph.open(SearchGraphFile);
  fileStreamWrittenSearchGraph.open(SearchGraphFileWritten);

  std::string lineA;
  std::string lineB;

  while (getline(fileStreamSearchGraph, lineA) &&
         getline(fileStreamWrittenSearchGraph, lineB)) {
    if (lineA != lineB) {
      fileStreamWrittenSearchGraph.close();
      fileStreamSearchGraph.close();
      if (remove(SearchGraphFileWritten.c_str()) != 0)
        std::cout << "Error deleting file" << std::endl;
      else
        std::cout << "File successfully deleted" << std::endl;
    }
    ASSERT_EQ(lineA, lineB);
  }

  fileStreamWrittenSearchGraph.close();
  fileStreamSearchGraph.close();
  if (remove(SearchGraphFileWritten.c_str()) != 0)
    std::cout << "Error deleting file" << std::endl;
  else
    std::cout << "File successfully deleted" << std::endl;
  ASSERT_TRUE(true);
}

TEST_F(SearchGraphLanguageModelTests, SearchGraphLanguageModelgetIdToSym) {
  SearchGraphLanguageModel sgraph;
  sgraph.read_model(SearchGraphFile);
  ASSERT_EQ(sgraph.getIdToSym(0), "-");
  ASSERT_EQ(sgraph.getIdToSym(592), "a");
  ASSERT_EQ(sgraph.getIdToSym(2440), "J^");
  ASSERT_EQ(sgraph.getIdToSym(1500000), "");
}

TEST_F(SearchGraphLanguageModelTests, SearchGraphLanguageModelgetIdToWord) {
  SearchGraphLanguageModel sgraph;
  sgraph.read_model(SearchGraphFile);
  ASSERT_EQ(sgraph.getIdToWord(0), "-");
  ASSERT_EQ(sgraph.getIdToWord(2487), "bueno");
  ASSERT_EQ(sgraph.getIdToWord(2529), "durante");
  ASSERT_EQ(sgraph.getIdToWord(2546), "empezamos");
}

}  // namespace
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
