/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef DECODER_H_
#define DECODER_H_

#include <AcousticModel.h>
#include <Sample.h>
#include <SearchGraphLanguageModel.h>

#include <memory>

class Decoder {
  std::unique_ptr<SearchGraphLanguageModel> sgraph;
  std::unique_ptr<AcousticModel> amodel;

 public:
  Decoder(std::unique_ptr<SearchGraphLanguageModel>& sgraph,
          std::unique_ptr<AcousticModel>& amodel);
  float decode(Sample sample);
};

#endif  // MIXTUREACOUSTICMODEL_H_
