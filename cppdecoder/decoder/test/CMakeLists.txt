cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

BUILD_UNIT_TEST(TEST_SRC src/decoder_test.cpp
                LIBS cppdecoder::Utils
                     cppdecoder::Decoder
                     cppdecoder::AcousticModel
                     cppdecoder::Sample
                     cppdecoder::SearchGraphLanguageModel
                LIBS_INCLUDE ${Utils_SOURCE_DIR}/include
                             ${AcousticModel_SOURCE_DIR}/include
                             ${Sample_SOURCE_DIR}/include
                             ${SearchGraphLanguageModel_SOURCE_DIR}/include
                             )