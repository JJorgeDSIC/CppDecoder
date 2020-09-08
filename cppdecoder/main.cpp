#include <AcousticModel.h>
#include <DGaussianAcousticModel.h>
#include <Decoder.h>
#include <MixtureAcousticModel.h>
#include <Sample.h>
#include <SearchGraphLanguageModel.h>
#include <TiedStatesAcousticModel.h>
#include <Utils.h>

int main() {
  SearchGraphLanguageModel sgraph;
  sgraph.read_model("/home/jjorge/PyDecoder/models/2.gram.graph");

  MixtureAcousticModel* mixturemodel = new MixtureAcousticModel(
      "/home/jjorge/PyDecoder/models/monophone_model_I32");

  Sample sample;

  sample.read_sample("/home/jjorge/PyDecoder/samples/AAFA0016.features");

  std::cout << sample.getDim() << std::endl;
  std::cout << sample.getNFrames() << std::endl;

  Decoder decoder(sgraph, mixturemodel, sample);
}
