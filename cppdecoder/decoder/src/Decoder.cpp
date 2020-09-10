#include <Decoder.h>

Decoder::Decoder(std::unique_ptr<SearchGraphLanguageModel>& sgraph,
                 std::unique_ptr<AcousticModel>& amodel) {
  this->sgraph = std::move(sgraph);
  this->amodel = std::move(amodel);
  std::vector<int> activesTemp(this->sgraph->getNStates(),
                               -1);  // Review this...
  actives = std::move(activesTemp);
}

float Decoder::decode(Sample sample) {
  // Init structs
  // HMMNodeManager
  // viterbi_init with the first frame
  // for each frame, perform:
  //  viterbi iteration

  // Final iteration

  // Get and print the results

  return this->amodel->calc_logprob("a", 0, sample.getFrame(0).getFeatures());
}

void Decoder::expand_search_graph_nodes(
    std::vector<SGNode>& searchgraph_null_nodes0) {
  // TODO
  std::cout << "TODO" << std::endl;
}

void Decoder::addNodeToSearchGraphNullNodes1(std::unique_ptr<SGNode>& node) {
  search_graph_null_nodes1.push_back(std::move(node));
  std::cout << "TODO" << std::endl;
}
void Decoder::addNodeToSearchGraphNodes1(SGNode node) {
  std::cout << "TODO" << std::endl;
}

void Decoder::insert_search_graph_node(std::unique_ptr<SGNode>& node) {
  // TODO
  const std::string& symbol = sgraph->getIdToSym(node->getStateId());
  const std::string& word = sgraph->getIdToWord(node->getStateId());

  bool insertWord = word != "-" && word != ">";

  if (symbol == "-") {
    // Inserts in sg_null_nodes1
    std::cout << "sg_null_nodes1" << std::endl;
  }
  //  else {
  //   // Insert in sg_nodes1
  //   std::cout << "sg_nodes1" << std::endl;
  // }

  // // Not visited yet
  if (actives[node->getStateId()] == -1) {
    std::cout << "Not visited" << std::endl;
    addNodeToSearchGraphNullNodes1(node);
    std::cout << search_graph_null_nodes1[0]->getStateId() << std::endl;

  } else {
    std::cout << "Visited" << std::endl;
  }
}