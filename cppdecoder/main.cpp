#include <AcousticModel.h>
#include <Decoder.h>
#include <MixtureAcousticModel.h>
#include <Sample.h>
#include <SearchGraphLanguageModel.h>
#include <Utils.h>

#include <memory>

int main() {
  std::unique_ptr<SearchGraphLanguageModel> sgraph(
      new SearchGraphLanguageModel());
  sgraph->read_model("/home/jjorge/PyDecoder/models/2.gram.graph");

  std::unique_ptr<AcousticModel> mixturemodel(new MixtureAcousticModel(
      "/home/jjorge/PyDecoder/models/monophone_model_I32"));

  std::cout << mixturemodel->getDim() << std::endl;

  Decoder decoder(std::move(sgraph), std::move(mixturemodel));

  Sample sample;

  WordHyp wordhyp(-1, "hello");

  wordhyp.showWordHyp();

  sample.read_sample("/home/jjorge/PyDecoder/samples/AAFA0016.features");

  std::cout << sample.getNFrames() << std::endl;

  std::cout << decoder.decode(sample) << std::endl;

  SearchGraphLanguageModel sgraph2;
  sgraph2.read_model("/home/jjorge/PyDecoder/models/2.gram.graph");

  std::cout << sgraph2.getStartState() << std::endl;

  //   std::unique_ptr<SGNode> sgnode(
  //       new SGNode(sgraph2.getStartState(), 0.0, 0.0, 0.0, 0));
  //   std::unique_ptr<SGNode> sgnode(
  //       new SGNode(sgraph2.getStartState(), 0.0, 0.0, 0.0, 0));

  //   decoder.insert_search_graph_node(std::move(sgnode));

  //   std::unique_ptr<SGNode> sgnode2(
  //       new SGNode(sgraph2.getStartState(), 0.1, 0.0, 0.0, 0));

  //   decoder.insert_search_graph_node(std::move(sgnode2));

  // std::unique_ptr<SGNode> sgnodeIni(new SGNode(6408, 0.1, 0.0, 0.0, 0));

//   std::unique_ptr<SGNode> sgnodeIni(
//       new SGNode(sgraph2.getStartState(), 0.0, 0.0, 0.0, 0));

//   decoder.addNodeToSearchGraphNullNodes0(std::move(sgnodeIni));

//   std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes0 =
//       decoder.getSearchGraphNullNodes0();

//   decoder.expand_search_graph_nodes(std::move(search_graph_null_nodes0));

//   std::vector<std::unique_ptr<SGNode>>& search_graph_null_nodes1 =
//       decoder.getSearchGraphNullNodes1();

//   const std::vector<std::unique_ptr<SGNode>>& search_graph_nodes1 =
//       decoder.getSearchGraphNodes1();

//   std::cout << "Nodes in null_nodes_1 " << std::endl;
//   decoder.printSGNodes(decoder.getSearchGraphNullNodes1());

//   std::cout << "Nodes in nodes_1 " << std::endl;
//   decoder.printSGNodes(decoder.getSearchGraphNodes1());
//   std::cout << "========" << std::endl;

//   decoder.expand_search_graph_nodes(
//       std::move(decoder.getSearchGraphNullNodes1()));

//   std::cout << "========" << std::endl;

//   std::cout << "Nodes in null_nodes_1 " << std::endl;
//   decoder.printSGNodes(decoder.getSearchGraphNullNodes1());

//   std::cout << "Nodes in nodes_1 " << std::endl;
//   decoder.printSGNodes(decoder.getSearchGraphNodes1());
//   std::cout << "========" << std::endl;

//   decoder.expand_search_graph_nodes(
//       std::move(decoder.getSearchGraphNullNodes1()));
 
//   std::cout << "Nodes in null_nodes_1 " << std::endl;
//   decoder.printSGNodes(decoder.getSearchGraphNullNodes1());

//   std::cout << "Nodes in nodes_1 " << std::endl;
//   decoder.printSGNodes(decoder.getSearchGraphNodes1());

//   std::cout << "========" << std::endl;
}
