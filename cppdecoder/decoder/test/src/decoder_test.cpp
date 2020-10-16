/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */
#include <AcousticModel.h>
#include <Decoder.h>
#include <MixtureAcousticModel.h>
#include <SearchGraphLanguageModel.h>
#include <Utils.h>
#include <stdio.h>

#include <iomanip>  // std::setprecision
#include <memory>

#include "gtest/gtest.h"

namespace {

// TODO: Create more test cases

class DecoderTests : public ::testing::Test {
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

TEST_F(DecoderTests, DecoderConstructor) { ASSERT_TRUE(true); }

TEST_F(DecoderTests, DecoderInsertSGNodeNull) {
  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes0 =
      decoder->getSearchGraphNullNodes0();

  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes1 =
      decoder->getSearchGraphNullNodes1();

  std::vector<int> actives = decoder->getActives();

  std::unique_ptr<SGNode> sgnode(new SGNode(0, 0.0, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(sgnode);

  search_graph_null_nodes1[0]->showState();

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.1, 0.0, 0.0, 0));

  decoder->insert_search_graph_node(sgnode2);

  search_graph_null_nodes1[0]->showState();

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.1);
}

TEST_F(DecoderTests, DecoderInsertSGWordNode) {
  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes0 =
      decoder->getSearchGraphNullNodes0();

  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes1 =
      decoder->getSearchGraphNullNodes1();

  std::vector<std::unique_ptr<SGNode>>& search_graph_nodes1 =
      decoder->getSearchGraphNodes1();

  std::vector<int> actives = decoder->getActives();

  std::unique_ptr<SGNode> sgnode(new SGNode(2405, 0.0, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(sgnode);

  search_graph_null_nodes1[0]->showState();

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.1, 0.0, 0.0, 0));

  decoder->insert_search_graph_node(sgnode2);

  search_graph_null_nodes1[0]->showState();

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.1);
}

TEST_F(DecoderTests, DecoderInsertSGDifferentNodes) {
  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes0 =
      decoder->getSearchGraphNullNodes0();

  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes1 =
      decoder->getSearchGraphNullNodes1();

  std::vector<std::unique_ptr<SGNode>>& search_graph_nodes1 =
      decoder->getSearchGraphNodes1();

  std::vector<int> actives = decoder->getActives();

  std::unique_ptr<SGNode> sgnode(new SGNode(2405, 0.0, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(sgnode);

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.2, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(sgnode2);

  search_graph_null_nodes1[0]->showState();
  search_graph_null_nodes1[1]->showState();

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.2);
}

TEST_F(DecoderTests, DecoderInsertSGDiffNodeAndNoUpdate) {
  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes0 =
      decoder->getSearchGraphNullNodes0();

  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes1 =
      decoder->getSearchGraphNullNodes1();

  std::vector<std::unique_ptr<SGNode>>& search_graph_nodes1 =
      decoder->getSearchGraphNodes1();

  std::vector<int> actives = decoder->getActives();

  std::unique_ptr<SGNode> sgnode(new SGNode(2405, 0.0, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(sgnode);

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.2, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(sgnode2);

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.2);

  std::unique_ptr<SGNode> sgnode3(new SGNode(0, 0.1, 0.0, 0.0, 0));

  decoder->insert_search_graph_node(sgnode3);

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.2);

  std::unique_ptr<SGNode> sgnode4(new SGNode(0, 0.3, 0.0, 0.0, 0));

  decoder->insert_search_graph_node(sgnode4);

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.3);

  std::unique_ptr<SGNode> sgnode5(new SGNode(2405, 0.3, 0.0, 0.0, 0));

  decoder->insert_search_graph_node(sgnode5);

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.3);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.3);

  ASSERT_EQ(search_graph_null_nodes1.size(), 2);
  ASSERT_EQ(search_graph_nodes1.size(), 0);
}

TEST_F(DecoderTests, DecoderInsertSGNodeInsSGNodes1) {
  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes0 =
      decoder->getSearchGraphNullNodes0();

  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes1 =
      decoder->getSearchGraphNullNodes1();

  std::vector<std::unique_ptr<SGNode>>& search_graph_nodes1 =
      decoder->getSearchGraphNodes1();

  std::unique_ptr<SGNode> sgnode(new SGNode(2404, 0.5, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(sgnode);

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 1);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), 0.5);
}

TEST_F(DecoderTests, DecoderInsertSGNodeInsAndUpdateSGNodes1) {
  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes0 =
      decoder->getSearchGraphNullNodes0();

  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes1 =
      decoder->getSearchGraphNullNodes1();

  std::vector<std::unique_ptr<SGNode>>& search_graph_nodes1 =
      decoder->getSearchGraphNodes1();

  std::unique_ptr<SGNode> sgnode(new SGNode(2404, 0.5, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(sgnode);

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 1);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), 0.5);

  std::unique_ptr<SGNode> sgnode2(new SGNode(2407, 0.3, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(sgnode2);

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 2);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), 0.5);
  ASSERT_FLOAT_EQ(search_graph_nodes1[1]->getLProb(), 0.3);

  std::unique_ptr<SGNode> sgnode3(new SGNode(2404, 0.3, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(sgnode3);

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 2);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), 0.5);
  ASSERT_FLOAT_EQ(search_graph_nodes1[1]->getLProb(), 0.3);

  std::unique_ptr<SGNode> sgnode4(new SGNode(2404, 0.6, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(sgnode4);

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 2);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), 0.6);
  ASSERT_FLOAT_EQ(search_graph_nodes1[1]->getLProb(), 0.3);
}

TEST_F(DecoderTests, DecoderaddNodeToSearchGraphNullNodes0) {
  std::unique_ptr<SGNode> sgnodeIni(new SGNode(startState, 0.0, 0.0, 0.0,
  0));

  std::unique_ptr<SGNode> sgnode1(new SGNode(6408, 0.1, 0.0, 0.0, 0));

  ASSERT_EQ(decoder->getSearchGraphNullNodes0().size(), 0);

  decoder->addNodeToSearchGraphNullNodes0(sgnodeIni);

  ASSERT_EQ(decoder->getSearchGraphNullNodes0().size(), 1);

  decoder->getSearchGraphNullNodes0()[0]->showState();

  decoder->addNodeToSearchGraphNullNodes0(sgnode1);

  ASSERT_EQ(decoder->getSearchGraphNullNodes0().size(), 2);

  decoder->getSearchGraphNullNodes0()[0]->showState();
  decoder->getSearchGraphNullNodes0()[1]->showState();
}

TEST_F(DecoderTests, DecoderExpandSGNodeStartNode) {
  std::unique_ptr<SGNode> sgnodeStart(new SGNode(startState, 0.0, 0.0, 0.0,
  0));

  decoder->addNodeToSearchGraphNullNodes0(sgnodeStart);

  decoder->expand_search_graph_nodes(decoder->getSearchGraphNullNodes0());

  ASSERT_FLOAT_EQ(decoder->getSearchGraphNullNodes1()[0]->getLProb(),
  -28.1341);
}

TEST_F(DecoderTests, DecoderExpandSGNodeExample1) {
  std::unique_ptr<SGNode> sgnode1(
      new SGNode(6408, -28.134100, -28.134100, 0.0, 0));
  // std::unique_ptr<SGNode> sgnode2(new SGNode(2440, 0.1, 0.0, 0.0, 0));

  decoder->addNodeToSearchGraphNullNodes0(sgnode1);

  decoder->expand_search_graph_nodes(decoder->getSearchGraphNullNodes0());

  ASSERT_FLOAT_EQ(decoder->getSearchGraphNullNodes1()[0]->getLProb(),
                  -2302.504100);

  std::vector<float> lprobs = {-28.134100, -39.120200, -39.120200,
  -39.120200,
                               -46.051700, -39.120200, -32.188800,
                               -39.120200, -46.051700, -32.188800,
                               -46.051700, -46.051700, -32.188800,
                               -32.188800, -39.120200, -46.051700,
                               -39.120200, -46.051700, -29.957300};

  for (auto i = 0; i < lprobs.size(); i++) {
    ASSERT_FLOAT_EQ(decoder->getSearchGraphNodes1()[i]->getLProb(),
    lprobs[i]);
  }
}

TEST_F(DecoderTests, DecoderExpandSGNodeExample2) {
  std::unique_ptr<SGNode> sgnode1(
      new SGNode(2, -2302.504100, -2302.504100, 0.0, 0));

  decoder->addNodeToSearchGraphNullNodes0(sgnode1);

  decoder->expand_search_graph_nodes(decoder->getSearchGraphNullNodes0());

  ASSERT_EQ(decoder->getSearchGraphNullNodes1().size(), 0);

  std::vector<float> lprobs = {
      -2326.583600, -2322.165300, -2337.569700, -2320.223700, -2348.555800,
      -2330.638200, -2332.461500, -2326.583600, -2326.583600, -2334.692900,
      -2315.233800, -2348.555800, -2332.461500, -2348.555800, -2318.110600,
      -2337.569700, -2337.569700, -2341.624400, -2341.624400, -2348.555800,
      -2315.974900, -2318.110600, -2311.920200, -2348.555800, -2348.555800,
      -2315.233800, -2332.461500, -2341.624400, -2317.200900};
  for (auto i = 0; i < lprobs.size(); i++) {
    ASSERT_FLOAT_EQ(decoder->getSearchGraphNodes1()[i]->getLProb(),
    lprobs[i]);
  }
}

TEST_F(DecoderTests, DecoderGetReadyNullNodes) {
  std::unique_ptr<SGNode> sgnode1(
      new SGNode(6408, -28.134100, -28.134100, 0.0, 0));
  std::unique_ptr<SGNode> sgnode2(
      new SGNode(2, -18.103909, -18.103909, 0.0, 0));

  decoder->addNodeToSearchGraphNullNodes1(sgnode1);
  decoder->addNodeToSearchGraphNullNodes1(sgnode2);

  ASSERT_FLOAT_EQ(decoder->getSearchGraphNullNodes1()[0]->getLProb(),
                  -28.134100);
  ASSERT_FLOAT_EQ(decoder->getSearchGraphNullNodes1()[1]->getLProb(),
                  -18.103909);

  ASSERT_TRUE(decoder->getSearchGraphNullNodes0().size() == 0);
  ASSERT_TRUE(decoder->getSearchGraphNullNodes1().size() != 0);

  decoder->getReadyNullNodes();

  ASSERT_FLOAT_EQ(decoder->getSearchGraphNullNodes0()[0]->getLProb(),
                  -28.134100);
  ASSERT_FLOAT_EQ(decoder->getSearchGraphNullNodes0()[1]->getLProb(),
                  -18.103909);

  ASSERT_TRUE(decoder->getSearchGraphNullNodes0().size() != 0);
  ASSERT_TRUE(decoder->getSearchGraphNullNodes1().size() == 0);
}

TEST_F(DecoderTests, DecoderGetReadyNodes) {
  std::unique_ptr<SGNode> sgnode1(
      new SGNode(6408, -28.134100, -28.134100, 0.0, 0));
  std::unique_ptr<SGNode> sgnode2(
      new SGNode(2, -18.103909, -18.103909, 0.0, 0));

  decoder->addNodeToSearchGraphNodes1(sgnode1);
  decoder->addNodeToSearchGraphNodes1(sgnode2);

  ASSERT_FLOAT_EQ(decoder->getSearchGraphNodes1()[0]->getLProb(),
  -28.134100);
  ASSERT_FLOAT_EQ(decoder->getSearchGraphNodes1()[1]->getLProb(),
  -18.103909);
}

TEST_F(DecoderTests, DecoderViterbiIterSG) {

  std::unique_ptr<SGNode> sgnodeIni(new SGNode(startState, 0.0, 0.0, 0.0, 0));

  decoder->insert_search_graph_node(sgnodeIni);

  decoder->viterbiIterSG(0);

  ASSERT_EQ(decoder->nodes0IsNotEmpty(), false);
  ASSERT_EQ(decoder->nodes1IsNotEmpty(), true);
  ASSERT_EQ(decoder->nullNodes0IsNotEmpty(), false);
  ASSERT_EQ(decoder->nullNodes1IsNotEmpty(), false);

  std::vector<float> lprobs = {
      -28.134100,   -39.120200,   -39.120200,   -39.120200,   -46.051700,
      -39.120200,   -32.188800,   -39.120200,   -46.051700,   -32.188800,
      -46.051700,   -46.051700,   -32.188800,   -32.188800,   -39.120200,
      -46.051700,   -39.120200,   -46.051700,   -29.957300,   -2326.583600,
      -2322.165300, -2337.569700, -2320.223700, -2348.555800, -2330.638200,
      -2332.461500, -2326.583600, -2326.583600, -2334.692900, -2315.233800,
      -2348.555800, -2332.461500, -2348.555800, -2318.110600, -2337.569700,
      -2337.569700, -2341.624400, -2341.624400, -2348.555800, -2315.974900,
      -2318.110600, -2311.920200, -2348.555800, -2348.555800, -2315.233800,
      -2332.461500, -2341.624400, -2317.200900};

  for (auto i = 0; i < lprobs.size(); i++) {
    ASSERT_FLOAT_EQ(decoder->getSearchGraphNodes1()[i]->getLProb(), lprobs[i]);
  }
}

}  // namespace
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
