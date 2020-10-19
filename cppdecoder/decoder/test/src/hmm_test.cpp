/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */
#include <AcousticModel.h>
#include <Decoder.h>
#include <HMM.h>
#include <MixtureAcousticModel.h>
#include <SearchGraphLanguageModel.h>
#include <Utils.h>
#include <stdio.h>

#include <algorithm>
#include <iomanip>  // std::setprecision
#include <iostream>
#include <iterator>
#include <memory>
#include <random>

#include "gtest/gtest.h"

namespace {

// TODO: Create more test cases

class HMMTests : public ::testing::Test {
 protected:
  const std::string nameModelMixture =
      "./models/mixture_monophoneme_I32.example.model";
  const std::string searchGraphFile = "./models/2.gram.graph";

  Decoder* decoder;
  size_t startState;

  void SetUp() override {
    std::unique_ptr<SearchGraphLanguageModel> sgraph(
        new SearchGraphLanguageModel());
    sgraph->read_model(searchGraphFile);
    startState = sgraph->getStartState();
    std::unique_ptr<AcousticModel> mixturemodel(
        new MixtureAcousticModel(nameModelMixture));

    decoder = new Decoder(std::move(sgraph), std::move(mixturemodel));
  }
};

// TEST_F(HMMTests, DecoderHMMActives) {
//   HMMNodeId nodeId = HMMNodeId(10, 1);
//   HMMNodeId nodeId2 = HMMNodeId(20, 1);

//   decoder->setHMMActive(nodeId, 10);

//   int position = decoder->getHMMPosIfActive(nodeId);
//   int position2 = decoder->getHMMPosIfActive(nodeId2);

//   std::cout << "Position: " << position << std::endl;
//   std::cout << "Position: " << position2 << std::endl;
// }

TEST_F(HMMTests, DecoderHMMAddAndExtractMin) {

  int capacity = 100;

  std::unique_ptr<HMMMinHeap> minHeap(new HMMMinHeap(capacity));

  std::vector<float> vec(capacity);
  // Generate 10 random numbers by lambda func and fill it in vector
  std::generate(vec.begin(), vec.end(), [&]() { return rand() % capacity * 2; });

  std::vector<float> sorted_vec;
  for (auto& value : vec) {
    sorted_vec.push_back(value);
  }
 
  std::sort(sorted_vec.begin(), sorted_vec.end());

  for (size_t i = 0; i < vec.size(); i++) {
    uint32_t sg_state = i;
    uint32_t hmm_q_state = 0;
    float lprob = vec[i];
    float hmmp = vec[i];
    float lmp = vec[i];
    uint32_t trapos = 0;
    uint32_t h = 0;

    std::unique_ptr<HMMNode> node(
        new HMMNode(sg_state, hmm_q_state, lprob, hmmp, lmp, trapos, h));

    minHeap->insert(std::move(node));
  }
  for (size_t i = 0; i < vec.size(); i++) {
    ASSERT_EQ(minHeap->getMinLProb(), sorted_vec[i]);
    std::unique_ptr<HMMNode> minNode = minHeap->extractMinLProbHMMNode();
    ASSERT_EQ(minNode->getLogProb(), sorted_vec[i]);
  }
}

// TEST_F(HMMTests, DecoderHMMPopAdd) { ASSERT_TRUE(false); }

// TEST_F(HMMTests, DecoderHMMUpdate) { ASSERT_TRUE(false); }

}  // namespace
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
