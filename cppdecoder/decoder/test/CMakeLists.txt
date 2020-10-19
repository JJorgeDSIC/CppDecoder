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


BUILD_UNIT_TEST(TEST_SRC src/hmm_test.cpp
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

configure_file(
     ${CMAKE_SOURCE_DIR}/models/2.gram.graph ${CMAKE_BINARY_DIR}/models/2.gram.graph COPYONLY)
                         
configure_file(
     ${CMAKE_SOURCE_DIR}/models/mixture_monophoneme_I32.example.model ${CMAKE_BINARY_DIR}/models/mixture_monophoneme_I32.example.model COPYONLY)
