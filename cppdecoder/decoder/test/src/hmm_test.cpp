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

TEST_F(HMMTests, DecoderHMMAddAndExtractMin) {
  int capacity = 100;
  std::unique_ptr<HMMMinHeap> minHeap(new HMMMinHeap(5));
  std::vector<float> vec(capacity);
  std::generate(vec.begin(), vec.end(),
                [&]() { return rand() % capacity * 2; });

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

TEST_F(HMMTests, DecoderHMMAddAndExtractMinAndGettingPosition) {
  int position = -1;
  int capacity = 100;
  std::unique_ptr<HMMMinHeap> minHeap(new HMMMinHeap(5));

  std::vector<float> vec = {190, 140, 68,  156, 134, 2, 194,
                            4,   34,  184, 104, 112, 2};
  std::vector<float> sorted_vec = {2,   2,   4,   34,  68,  104, 112,
                                   134, 140, 156, 184, 190, 194};

  std::vector<int> vec_posis_remove1 = {8, 12, 6,  11, 9,  1, 7,
                                        2, 4,  10, 5,  13, 3};

  std::vector<int> vec_posis_remove2 = {8, 12, 3,  11, 9, -1, 7,
                                        2, 4,  10, 5,  6, 1};

  std::vector<int> vec_posis_remove3 = {8, 9, 3,  11, 4, -1, 7,
                                        1, 2, 10, 5,  6, -1};

  std::vector<int> vec_posis_remove4 = {8,  9, 3,  5, 4, -1, 7,
                                        -1, 1, 10, 2,  6, -1};

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

    position = minHeap->insert(std::move(node));

  }

  for (size_t i = 0; i < vec.size(); i++) {
    int positionInside = minHeap->getNodePositionById(i, 0);
    ASSERT_EQ(vec_posis_remove1[i], positionInside);
  }

  ASSERT_EQ(minHeap->getMinLProb(), sorted_vec[0]);
  std::unique_ptr<HMMNode> minNode = minHeap->extractMinLProbHMMNode();
  ASSERT_EQ(minNode->getLogProb(), sorted_vec[0]);

  for (size_t i = 0; i < vec.size(); i++) {
    int positionInside = minHeap->getNodePositionById(i, 0);
    ASSERT_EQ(vec_posis_remove2[i], positionInside);
  }

  ASSERT_EQ(minHeap->getMinLProb(), sorted_vec[1]);
  ASSERT_EQ(minHeap->extractMinLProbHMMNode()->getLogProb(), sorted_vec[1]);

  for (size_t i = 0; i < vec.size(); i++) {
    int positionInside = minHeap->getNodePositionById(i, 0);
    ASSERT_EQ(vec_posis_remove3[i], positionInside);
  }

  ASSERT_EQ(minHeap->getMinLProb(), sorted_vec[2]);
  ASSERT_EQ(minHeap->extractMinLProbHMMNode()->getLogProb(), sorted_vec[2]);

  for (size_t i = 0; i < vec.size(); i++) {
    int positionInside = minHeap->getNodePositionById(i, 0);
    ASSERT_EQ(vec_posis_remove4[i], positionInside);
  }

} 
TEST_F(HMMTests, DecoderHMMUpdate) {
  int position = -1;
  int capacity = 100;
  std::unique_ptr<HMMMinHeap> minHeap(new HMMMinHeap(5));

  std::vector<float> vec = {190, 140, 68,  156, 134, 2, 194,
                            4,   34,  184, 104, 112, 2};

  std::vector<float> sorted_vec = {2,   2,   4,   34,  68,  104, 112,
                                   134, 140, 156, 184, 190, 194};

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

    position = minHeap->insert(std::move(node));
  }

  minHeap->update(8, 0, 1);

  ASSERT_EQ(minHeap->getMinLProb(), 1);

  minHeap->update(11, 0, 2);

  ASSERT_EQ(minHeap->getMinLProb(), 1);

  minHeap->update(0, 0, 0);

  ASSERT_EQ(minHeap->getMinLProb(), 0);
}

TEST_F(HMMTests, DecoderHMMUpdateAt) {
  int position = -1;
  int capacity = 100;
  std::unique_ptr<HMMMinHeap> minHeap(new HMMMinHeap(5));

  std::vector<float> vec = {190, 140, 68,  156, 134, 2, 194,
                            4,   34,  184, 104, 112, 2};

  std::vector<float> sorted_vec = {2,   2,   4,   34,  68,  104, 112,
                                   134, 140, 156, 184, 190, 194};

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

    position = minHeap->insert(std::move(node));
  }

  // TODO
  position = minHeap->getNodePositionById(8, 0);

  minHeap->updateNodeAt(position, 1, 0.2);

  ASSERT_EQ(minHeap->getMinLProb(), 1);

  position = minHeap->getNodePositionById(11, 0);

  minHeap->updateNodeAt(position, 2, 0.3);

  ASSERT_EQ(minHeap->getMinLProb(), 1);

  position = minHeap->getNodePositionById(0, 0);

  minHeap->updateNodeAt(position, 0, 0.4);

  ASSERT_EQ(minHeap->getMinLProb(), 0);
}

TEST_F(HMMTests, DecoderHMMPopAdd) {
  int position = -1;
  int capacity = 100;
  std::unique_ptr<HMMMinHeap> minHeap(new HMMMinHeap(5));

  std::vector<float> vec = {190, 140, 68,  156, 134, 2, 194,
                            4,   34,  184, 104, 112, 2};

  std::vector<float> sorted_vec = {2,   2,   4,   34,  68,  104, 112,
                                   134, 140, 156, 184, 190, 194};

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

    position = minHeap->insert(std::move(node));
  }

  std::vector<float> vecPop = {1, 200, 50};
  std::vector<float> minProbOnEachPopAndInsertBefore = {2, 1, 2};
  std::vector<float> minProbOnEachPopAndInsertAfter = {1, 2, 4};

  for (size_t i = 0; i < vecPop.size(); i++) {
    uint32_t sg_state = vec.size() + i;
    uint32_t hmm_q_state = 0;
    float lprob = vecPop[i];
    float hmmp = vecPop[i];
    float lmp = vecPop[i];
    uint32_t trapos = 0;
    uint32_t h = 0;

    std::unique_ptr<HMMNode> node(
        new HMMNode(sg_state, hmm_q_state, lprob, hmmp, lmp, trapos, h));

    ASSERT_EQ(minProbOnEachPopAndInsertBefore[i], minHeap->getMinLProb());

    minHeap->popAndInsert(std::move(node));

    ASSERT_EQ(minProbOnEachPopAndInsertAfter[i], minHeap->getMinLProb());
  }
}
}  // namespace
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
