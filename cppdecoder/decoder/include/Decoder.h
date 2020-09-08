/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

#ifndef DECODER_H_
#define DECODER_H_

#include <AcousticModel.h>
#include <Sample.h>
#include <SearchGraphLanguageModel.h>

class Decoder {
 public:
  Decoder(SearchGraphLanguageModel sgraph, AcousticModel* amodel,
          Sample sample);
};

#endif  // MIXTUREACOUSTICMODEL_H_
