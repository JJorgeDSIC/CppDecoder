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

  Decoder decoder(sgraph, mixturemodel);

  Sample sample;

  WordHyp wordhyp(-1, "hello");

  wordhyp.showWordHyp();

  sample.read_sample("/home/jjorge/PyDecoder/samples/AAFA0016.features");

  std::cout << sample.getNFrames() << std::endl;

  std::cout << decoder.decode(sample) << std::endl;

  SearchGraphLanguageModel sgraph2;
  sgraph2.read_model("/home/jjorge/PyDecoder/models/2.gram.graph");


  std::cout << sgraph2.getStartState() << std::endl;

  std::unique_ptr<SGNode> sgnode(
      new SGNode(sgraph2.getStartState(), 0.0, 0.0, 0.0, 0));

  decoder.insert_search_graph_node(sgnode);
}
