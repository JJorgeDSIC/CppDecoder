#include <Decoder.h>

Decoder::Decoder(SearchGraphLanguageModel sgraph, AcousticModel* amodel, Sample sample) {
  std::cout << "Constructor..." << std::endl;

  float prob = amodel->calc_logprob("a", 0, sample.getFrame(0).getFeatures());

  std::cout << "Prob: " << prob << std::endl;
}