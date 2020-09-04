cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

BUILD_UNIT_TEST(TEST_SRC src/search_graph_language_model_test.cpp
                LIBS cppdecoder::Utils
                     cppdecoder::Sample
                LIBS_INCLUDE ${Utils_SOURCE_DIR}/include
                             ${SearchGraphLanguageModel_SOURCE_DIR}/include)


configure_file(
     ${CMAKE_SOURCE_DIR}/models/2.gram.graph ${CMAKE_BINARY_DIR}/test/models/2.gram.graph COPYONLY)
