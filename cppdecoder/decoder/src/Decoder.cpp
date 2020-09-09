#include <Decoder.h>

Decoder::Decoder(std::unique_ptr<SearchGraphLanguageModel>& sgraph,
                 std::unique_ptr<AcousticModel>& amodel) {
  this->sgraph = std::move(sgraph);
  this->amodel = std::move(amodel);
}

float Decoder::decode(Sample sample) {
  return this->amodel->calc_logprob("a", 0, sample.getFrame(0).getFeatures());
}