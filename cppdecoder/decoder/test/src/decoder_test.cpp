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

  void SetUp() override {
    std::unique_ptr<SearchGraphLanguageModel> sgraph(
        new SearchGraphLanguageModel());
    sgraph->read_model(searchGraphFile);

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
  decoder->insert_search_graph_node(std::move(sgnode));

  search_graph_null_nodes1[0]->showState();

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.1, 0.0, 0.0, 0));

  decoder->insert_search_graph_node(std::move(sgnode2));

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
  decoder->insert_search_graph_node(std::move(sgnode));

  search_graph_null_nodes1[0]->showState();

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.1, 0.0, 0.0, 0));

  decoder->insert_search_graph_node(std::move(sgnode2));

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
  decoder->insert_search_graph_node(std::move(sgnode));

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.2, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(std::move(sgnode2));

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
  decoder->insert_search_graph_node(std::move(sgnode));

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.2, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(std::move(sgnode2));

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.2);

  std::unique_ptr<SGNode> sgnode3(new SGNode(0, 0.1, 0.0, 0.0, 0));

  decoder->insert_search_graph_node(std::move(sgnode3));

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.2);

  std::unique_ptr<SGNode> sgnode4(new SGNode(0, 0.3, 0.0, 0.0, 0));

  decoder->insert_search_graph_node(std::move(sgnode4));

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.3);

  std::unique_ptr<SGNode> sgnode5(new SGNode(2405, 0.3, 0.0, 0.0, 0));

  decoder->insert_search_graph_node(std::move(sgnode5));

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.3);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.3);

  ASSERT_EQ(search_graph_null_nodes1.size(), 2);
  ASSERT_EQ(search_graph_nodes1.size(), 0);
}

TEST_F(DecoderTests, DecoderInsertSGNodeInsGNodes1) {
  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes0 =
      decoder->getSearchGraphNullNodes0();

  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes1 =
      decoder->getSearchGraphNullNodes1();

  std::vector<std::unique_ptr<SGNode>>& search_graph_nodes1 =
      decoder->getSearchGraphNodes1();

  std::unique_ptr<SGNode> sgnode(new SGNode(2404, 0.5, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(std::move(sgnode));

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 1);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), 0.5);
}

TEST_F(DecoderTests, DecoderInsertSGNodeInsAndUpdateGNodes1) {
  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes0 =
      decoder->getSearchGraphNullNodes0();

  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes1 =
      decoder->getSearchGraphNullNodes1();

  std::vector<std::unique_ptr<SGNode>>& search_graph_nodes1 =
      decoder->getSearchGraphNodes1();

  std::unique_ptr<SGNode> sgnode(new SGNode(2404, 0.5, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(std::move(sgnode));

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 1);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), 0.5);

  std::unique_ptr<SGNode> sgnode2(new SGNode(2407, 0.3, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(std::move(sgnode2));

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 2);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), 0.5);
  ASSERT_FLOAT_EQ(search_graph_nodes1[1]->getLProb(), 0.3);

  std::unique_ptr<SGNode> sgnode3(new SGNode(2404, 0.3, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(std::move(sgnode3));

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 2);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), 0.5);
  ASSERT_FLOAT_EQ(search_graph_nodes1[1]->getLProb(), 0.3);

  std::unique_ptr<SGNode> sgnode4(new SGNode(2404, 0.6, 0.0, 0.0, 0));
  decoder->insert_search_graph_node(std::move(sgnode4));

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 2);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), 0.6);
  ASSERT_FLOAT_EQ(search_graph_nodes1[1]->getLProb(), 0.3);
}

}  // namespace
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
