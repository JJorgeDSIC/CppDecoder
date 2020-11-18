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

#include <iomanip>  // std::setprecision
#include <memory>

#include "gtest/gtest.h"

namespace {

// TODO: Create more test cases and document the test cases

class DecoderTests : public ::testing::Test {
 protected:
  const std::string nameModelMixture =
      "./models/mixture_monophoneme_I32.example.model";
  const std::string searchGraphFile = "./models/2.gram.graph";

  const std::string sampleFile = "./samples/AAFA0016.features";

  Sample sample;

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

    sample.read_sample(sampleFile);
  }
};

TEST_F(DecoderTests, DecoderConstructor) { ASSERT_TRUE(true); }

TEST_F(DecoderTests, DecoderInsertSGNodeNull) {
  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes0 =
      decoder->getSearchGraphNullNodes0();

  std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes1 =
      decoder->getSearchGraphNullNodes1();

  std::unique_ptr<SGNode> sgnode(new SGNode(0, 0.0, 0.0, 0.0, 0));
  decoder->insertSearchGraphNode(sgnode);

  search_graph_null_nodes1[0]->showState();

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.1, 0.0, 0.0, 0));

  decoder->insertSearchGraphNode(sgnode2);

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

  std::unique_ptr<SGNode> sgnode(new SGNode(2405, 0.0, 0.0, 0.0, 0));
  decoder->insertSearchGraphNode(sgnode);

  search_graph_null_nodes1[0]->showState();

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.1, 0.0, 0.0, 0));

  decoder->insertSearchGraphNode(sgnode2);

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

  std::unique_ptr<SGNode> sgnode(new SGNode(2405, 0.0, 0.0, 0.0, 0));
  decoder->insertSearchGraphNode(sgnode);

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.2, 0.0, 0.0, 0));
  decoder->insertSearchGraphNode(sgnode2);

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

  std::unique_ptr<SGNode> sgnode(new SGNode(2405, 0.0, 0.0, 0.0, 0));
  decoder->insertSearchGraphNode(sgnode);

  std::unique_ptr<SGNode> sgnode2(new SGNode(0, 0.2, 0.0, 0.0, 0));
  decoder->insertSearchGraphNode(sgnode2);

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.2);

  std::unique_ptr<SGNode> sgnode3(new SGNode(0, 0.1, 0.0, 0.0, 0));

  decoder->insertSearchGraphNode(sgnode3);

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.2);

  std::unique_ptr<SGNode> sgnode4(new SGNode(0, 0.3, 0.0, 0.0, 0));

  decoder->insertSearchGraphNode(sgnode4);

  ASSERT_FLOAT_EQ(search_graph_null_nodes1[0]->getLProb(), 0.0);
  ASSERT_FLOAT_EQ(search_graph_null_nodes1[1]->getLProb(), 0.3);

  std::unique_ptr<SGNode> sgnode5(new SGNode(2405, 0.3, 0.0, 0.0, 0));

  decoder->insertSearchGraphNode(sgnode5);

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
  decoder->insertSearchGraphNode(sgnode);

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

  // Symbol node 'a' with logprob
  std::unique_ptr<SGNode> sgnode(new SGNode(2404, log(0.5), 0.0, 0.0, 0));
  decoder->insertSearchGraphNode(sgnode);

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 1);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), log(0.5));

  // Pruned by v_lm_thr
  // Symbol node 'b' with logprob
  std::unique_ptr<SGNode> sgnode2_1(new SGNode(2407, log(0.3), 0.0, 0.0, 0));
  decoder->insertSearchGraphNode(sgnode2_1);

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 2);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), log(0.5));

  // Symbol node 'b' with logprob
  std::unique_ptr<SGNode> sgnode2_2(new SGNode(2407, log(0.7), 0.0, 0.0, 0));
  decoder->insertSearchGraphNode(sgnode2_2);

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 2);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), log(0.5));
  ASSERT_FLOAT_EQ(search_graph_nodes1[1]->getLProb(), log(0.7));

  std::unique_ptr<SGNode> sgnode3(new SGNode(2404, log(0.3), 0.0, 0.0, 0));
  decoder->insertSearchGraphNode(sgnode3);

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 2);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), log(0.5));
  ASSERT_FLOAT_EQ(search_graph_nodes1[1]->getLProb(), log(0.7));

  std::unique_ptr<SGNode> sgnode4(new SGNode(2404, log(0.8), 0.0, 0.0, 0));
  decoder->insertSearchGraphNode(sgnode4);

  ASSERT_EQ(search_graph_null_nodes1.size(), 0);
  ASSERT_EQ(search_graph_nodes1.size(), 2);
  ASSERT_FLOAT_EQ(search_graph_nodes1[0]->getLProb(), log(0.8));
  ASSERT_FLOAT_EQ(search_graph_nodes1[1]->getLProb(), log(0.7));
}

TEST_F(DecoderTests, DecoderaddNodeToSearchGraphNullNodes0) {
  std::unique_ptr<SGNode> sgnodeIni(new SGNode(startState, 0.0, 0.0, 0.0, 0));

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
  std::unique_ptr<SGNode> sgnodeStart(new SGNode(startState, 0.0, 0.0, 0.0, 0));

  decoder->addNodeToSearchGraphNullNodes0(sgnodeStart);

  decoder->expandSearchGraphNodes(decoder->getSearchGraphNullNodes0());

  ASSERT_FLOAT_EQ(decoder->getSearchGraphNullNodes1()[0]->getLProb(), -28.1341);
}

TEST_F(DecoderTests, DecoderExpandSGNodeExample1) {
  std::unique_ptr<SGNode> sgnode1(
      new SGNode(6408, -28.134100, -28.134100, 0.0, 0));
  // std::unique_ptr<SGNode> sgnode2(new SGNode(2440, 0.1, 0.0, 0.0, 0));

  decoder->addNodeToSearchGraphNullNodes0(sgnode1);

  decoder->expandSearchGraphNodes(decoder->getSearchGraphNullNodes0());

  ASSERT_FLOAT_EQ(decoder->getSearchGraphNullNodes1()[0]->getLProb(),
                  -2302.504100);

  std::vector<float> lprobs = {-28.134100, -39.120200, -39.120200, -39.120200,
                               -46.051700, -39.120200, -32.188800, -39.120200,
                               -46.051700, -32.188800, -46.051700, -46.051700,
                               -32.188800, -32.188800, -39.120200, -46.051700,
                               -39.120200, -46.051700, -29.957300};

  for (auto i = 0; i < lprobs.size(); i++) {
    ASSERT_FLOAT_EQ(decoder->getSearchGraphNodes1()[i]->getLProb(), lprobs[i]);
  }
}

TEST_F(DecoderTests, DecoderExpandSGNodeExample2) {
  std::unique_ptr<SGNode> sgnode1(
      new SGNode(2, -2302.504100, -2302.504100, 0.0, 0));

  decoder->addNodeToSearchGraphNullNodes0(sgnode1);

  decoder->expandSearchGraphNodes(decoder->getSearchGraphNullNodes0());

  ASSERT_EQ(decoder->getSearchGraphNullNodes1().size(), 0);

  std::vector<float> lprobs = {
      -2326.583600, -2322.165300, -2337.569700, -2320.223700, -2348.555800,
      -2330.638200, -2332.461500, -2326.583600, -2326.583600, -2334.692900,
      -2315.233800, -2348.555800, -2332.461500, -2348.555800, -2318.110600,
      -2337.569700, -2337.569700, -2341.624400, -2341.624400, -2348.555800,
      -2315.974900, -2318.110600, -2311.920200, -2348.555800, -2348.555800,
      -2315.233800, -2332.461500, -2341.624400, -2317.200900};
  for (auto i = 0; i < lprobs.size(); i++) {
    ASSERT_FLOAT_EQ(decoder->getSearchGraphNodes1()[i]->getLProb(), lprobs[i]);
  }
}

TEST_F(DecoderTests, DecoderExpandSGNodeNotFinalIterAndDSTFinalState) {
  std::unique_ptr<SGNode> sgnode1(
      new SGNode(2, -2302.504100, -2302.504100, 0.0, 0));

  decoder->addNodeToSearchGraphNullNodes0(sgnode1);

  decoder->expandSearchGraphNodes(decoder->getSearchGraphNullNodes0());

  ASSERT_EQ(decoder->getSearchGraphNullNodes1().size(), 0);
  ASSERT_EQ(decoder->getSearchGraphNodes1().size(), 29);

  for (auto& node : decoder->getSearchGraphNodes1()) {
    ASSERT_TRUE(node->getStateId() != 2);
  }
}

TEST_F(DecoderTests, DecoderExpandSGNodeFinalIterAndNullNode) {
  std::unique_ptr<SGNode> sgnode1(
      new SGNode(2, -2302.504100, -2302.504100, 0.0, 0));

  decoder->setFinalIter(true);

  decoder->addNodeToSearchGraphNullNodes0(sgnode1);

  decoder->expandSearchGraphNodes(decoder->getSearchGraphNullNodes0());

  ASSERT_EQ(decoder->getSearchGraphNullNodes1().size(), 1);
  ASSERT_EQ(decoder->getSearchGraphNodes1().size(), 0);

  for (auto& node : decoder->getSearchGraphNullNodes1()) {
    ASSERT_TRUE(node->getStateId() == 1);
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

  ASSERT_FLOAT_EQ(decoder->getSearchGraphNodes1()[0]->getLProb(), -28.134100);
  ASSERT_FLOAT_EQ(decoder->getSearchGraphNodes1()[1]->getLProb(), -18.103909);
}

TEST_F(DecoderTests, DecoderViterbiIterSG) {
  std::unique_ptr<SGNode> sgnodeIni(new SGNode(startState, 0.0, 0.0, 0.0, 0));

  decoder->insertSearchGraphNode(sgnodeIni);

  decoder->viterbiIterSG(0);

  ASSERT_EQ(decoder->nodes0IsNotEmpty(), true);
  ASSERT_EQ(decoder->nodes1IsNotEmpty(), false);
  ASSERT_EQ(decoder->nullNodes0IsNotEmpty(), true);
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
    ASSERT_FLOAT_EQ(decoder->getSearchGraphNodes0()[i]->getLProb(), lprobs[i]);
  }
}

TEST_F(DecoderTests, DecoderViterbiSG2HMM_test1) {
  std::unique_ptr<SGNode> sgnodeIni(new SGNode(startState, 0.0, 0.0, 0.0, 0));

  std::unordered_map<int, float> gtruthLogProb = {
      {429, -2348.555800}, {438, -2348.555800}, {436, -2348.555800},
      {444, -2348.555800}, {448, -2348.555800}, {431, -2332.461500},
      {427, -2337.569700}, {437, -2332.461500}, {442, -2341.624400},
      {426, -2322.165300}, {449, -2348.555800}, {453, -2317.200900},
      {430, -2330.638200}, {433, -2326.583600}, {434, -2334.692900},
      {2455, -46.051700},  {425, -2326.583600}, {441, -2337.569700},
      {443, -2341.624400}, {445, -2315.974900}, {446, -2318.110600},
      {428, -2320.223700}, {452, -2341.624400}, {2451, -46.051700},
      {2442, -39.120200},  {2452, -32.188800},  {432, -2326.583600},
      {2453, -32.188800},  {2454, -39.120200},  {2446, -32.188800},
      {435, -2315.233800}, {2440, -28.134100},  {2441, -39.120200},
      {2456, -39.120200},  {439, -2318.110600}, {2447, -39.120200},
      {2457, -46.051700},  {2458, -29.957300},  {440, -2337.569700},
      {2449, -32.188800},  {2448, -46.051700},  {2450, -46.051700},
      {447, -2311.920200}, {2443, -39.120200},  {450, -2315.233800},
      {2444, -46.051700},  {451, -2332.461500}, {2445, -39.120200}};

  std::unordered_map<int, float> gtruthLMLogProb = {
      {429, -234.855580}, {438, -234.855580}, {436, -234.855580},
      {444, -234.855580}, {448, -234.855580}, {431, -233.246150},
      {427, -233.756970}, {437, -233.246150}, {442, -234.162440},
      {426, -232.216530}, {449, -234.855580}, {453, -231.720090},
      {430, -233.063820}, {433, -232.658360}, {434, -233.469290},
      {2455, -4.605170},  {425, -232.658360}, {441, -233.756970},
      {443, -234.162440}, {445, -231.597490}, {446, -231.811060},
      {428, -232.022370}, {452, -234.162440}, {2451, -4.605170},
      {2442, -3.912020},  {2452, -3.218880},  {432, -232.658360},
      {2453, -3.218880},  {2454, -3.912020},  {2446, -3.218880},
      {435, -231.523380}, {2440, -2.813410},  {2441, -3.912020},
      {2456, -3.912020},  {439, -231.811060}, {2447, -3.912020},
      {2457, -4.605170},  {2458, -2.995730},  {440, -233.756970},
      {2449, -3.218880},  {2448, -4.605170},  {2450, -4.605170},
      {447, -231.192020}, {2443, -3.912020},  {450, -231.523380},
      {2444, -4.605170},  {451, -233.246150}, {2445, -3.912020}};

  decoder->insertSearchGraphNode(sgnodeIni);

  decoder->viterbiIterSG(0);

  decoder->viterbiSg2HMM(sample);

  ASSERT_EQ(decoder->getNumberActiveHMMNodes1(), 48);

  for (size_t i = 0; i < decoder->getNumberActiveHMMNodes1(); i++) {
    ASSERT_FLOAT_EQ(
        gtruthLogProb[decoder->getHMMNodes1()[i + 1]->getId().sg_state],
        decoder->getHMMNodes1()[i + 1]->getLogProb());
    ASSERT_FLOAT_EQ(
        gtruthLMLogProb[decoder->getHMMNodes1()[i + 1]->getId().sg_state],
        decoder->getHMMNodes1()[i + 1]->getLMLogProb());
  }
}

TEST_F(DecoderTests, DecoderViterbiInit) {
  std::unordered_map<int, float> gtruthLogProb = {
      {429, -2348.555800}, {438, -2348.555800}, {436, -2348.555800},
      {444, -2348.555800}, {448, -2348.555800}, {431, -2332.461500},
      {427, -2337.569700}, {437, -2332.461500}, {442, -2341.624400},
      {426, -2322.165300}, {449, -2348.555800}, {453, -2317.200900},
      {430, -2330.638200}, {433, -2326.583600}, {434, -2334.692900},
      {2455, -46.051700},  {425, -2326.583600}, {441, -2337.569700},
      {443, -2341.624400}, {445, -2315.974900}, {446, -2318.110600},
      {428, -2320.223700}, {452, -2341.624400}, {2451, -46.051700},
      {2442, -39.120200},  {2452, -32.188800},  {432, -2326.583600},
      {2453, -32.188800},  {2454, -39.120200},  {2446, -32.188800},
      {435, -2315.233800}, {2440, -28.134100},  {2441, -39.120200},
      {2456, -39.120200},  {439, -2318.110600}, {2447, -39.120200},
      {2457, -46.051700},  {2458, -29.957300},  {440, -2337.569700},
      {2449, -32.188800},  {2448, -46.051700},  {2450, -46.051700},
      {447, -2311.920200}, {2443, -39.120200},  {450, -2315.233800},
      {2444, -46.051700},  {451, -2332.461500}, {2445, -39.120200}};

  std::unordered_map<int, float> gtruthLMLogProb = {
      {429, -234.855580}, {438, -234.855580}, {436, -234.855580},
      {444, -234.855580}, {448, -234.855580}, {431, -233.246150},
      {427, -233.756970}, {437, -233.246150}, {442, -234.162440},
      {426, -232.216530}, {449, -234.855580}, {453, -231.720090},
      {430, -233.063820}, {433, -232.658360}, {434, -233.469290},
      {2455, -4.605170},  {425, -232.658360}, {441, -233.756970},
      {443, -234.162440}, {445, -231.597490}, {446, -231.811060},
      {428, -232.022370}, {452, -234.162440}, {2451, -4.605170},
      {2442, -3.912020},  {2452, -3.218880},  {432, -232.658360},
      {2453, -3.218880},  {2454, -3.912020},  {2446, -3.218880},
      {435, -231.523380}, {2440, -2.813410},  {2441, -3.912020},
      {2456, -3.912020},  {439, -231.811060}, {2447, -3.912020},
      {2457, -4.605170},  {2458, -2.995730},  {440, -233.756970},
      {2449, -3.218880},  {2448, -4.605170},  {2450, -4.605170},
      {447, -231.192020}, {2443, -3.912020},  {450, -231.523380},
      {2444, -4.605170},  {451, -233.246150}, {2445, -3.912020}};

  std::vector<float> orderedValues = {
      -2348.555800, -2348.555800, -2348.555800, -2348.555800, -2348.555800,
      -2332.461500, -2337.569700, -2332.461500, -2341.624400, -2322.165300,
      -2348.555800, -2317.200900, -2330.638200, -2326.583600, -2334.692900,
      -46.051700,   -2326.583600, -2337.569700, -2341.624400, -2315.974900,
      -2318.110600, -2320.223700, -2341.624400, -46.051700,   -39.120200,
      -32.188800,   -2326.583600, -32.188800,   -39.120200,   -32.188800,
      -2315.233800, -28.134100,   -39.120200,   -39.120200,   -2318.110600,
      -39.120200,   -46.051700,   -29.957300,   -2337.569700, -32.188800,
      -46.051700,   -46.051700,   -2311.920200, -39.120200,   -2315.233800,
      -46.051700,   -2332.461500, -39.120200};

  std::vector<int> orderedIds = {
      429,  438,  436, 444,  448,  431,  427, 437,  442,  426,  449, 453,
      430,  433,  434, 2455, 425,  441,  443, 445,  446,  428,  452, 2451,
      2442, 2452, 432, 2453, 2454, 2446, 435, 2440, 2441, 2456, 439, 2447,
      2457, 2458, 440, 2449, 2448, 2450, 447, 2443, 450,  2444, 451, 2445};

  // std::unique_ptr<SGNode> sgnodeIni(new
  // SGNode(startState, 0.0, 0.0, 0.0, 0));

  // decoder->insertSearchGraphNode(sgnodeIni);

  // decoder->viterbiIterSG(0);

  // std::cout << "Viterbisg2HMM" << std::endl;

  // decoder->viterbiSg2HMM(sample);

  // decoder->getReadyHMMNodes0();

  decoder->viterbiInit(sample);

  ASSERT_EQ(decoder->getNumberActiveHMMNodes0(), 48);

  for (size_t i = 0; i < decoder->getNumberActiveHMMNodes0(); i++) {
    ASSERT_FLOAT_EQ(
        gtruthLogProb[decoder->getHMMNodes0()[i + 1]->getId().sg_state],
        decoder->getHMMNodes0()[i + 1]->getLogProb());
    ASSERT_FLOAT_EQ(
        gtruthLMLogProb[decoder->getHMMNodes0()[i + 1]->getId().sg_state],
        decoder->getHMMNodes0()[i + 1]->getLMLogProb());

    ASSERT_FLOAT_EQ(orderedValues[i],
                    decoder->getHMMNodes0()[i + 1]->getLogProb());

    ASSERT_EQ(orderedIds[i], decoder->getHMMNodes0()[i + 1]->getId().sg_state);
  }

  ASSERT_EQ(decoder->getNumberActiveHMMNodes1(), 0);
}

TEST_F(DecoderTests, DecoderViterbiIter_test1) {
  decoder->viterbiInit(sample);
  // TODO: Fix prune before case
  // TODO: Fix adaptative beam

  std::cout << "# of frames: " << sample.getNFrames() << std::endl;

  decoder->getWordHyps().emplace_back(-1, "");
  decoder->setVLMBeam(HUGE_VAL);  // LM rescoring related...
  decoder->setVBeam(300);
  for (size_t i = 0; i < sample.getNFrames() - 1; i++) {
    decoder->viterbiIter(sample, i, false);
  }

  decoder->viterbiIter(sample, sample.getNFrames() - 1, true);

  ASSERT_EQ("la sangre se revuelve con estas migas de pan blanco ",
            decoder->getResult());
}

TEST_F(DecoderTests, DecoderDecode) {
  decoder->decode(sample);

  std::cout << decoder->getResult() << std::endl;

  ASSERT_EQ("la sangre se revuelve con estas migas de pan blanco ",
            decoder->getResult());

  decoder->resetDecoder();

  const std::string sampleFile_local = "./samples/AAFA0002.features";

  Sample sample_local;

  sample_local.read_sample(sampleFile_local);

  decoder->decode(sample_local);

  std::cout << decoder->getResult() << std::endl;

  ASSERT_EQ("mi primer profesor de lengua fue lopez garcia ",
            decoder->getResult());
}

}  // namespace
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
