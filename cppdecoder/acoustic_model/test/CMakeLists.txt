cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

configure_file(
  ${CMAKE_SOURCE_DIR}/models/dgaussian_monopohoneme_I01.example.model ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/models/dgaussian_monopohoneme_I01.example.model COPYONLY)

configure_file(
  ${CMAKE_SOURCE_DIR}/models/mixture_monophoneme_I32.example.model ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/models/mixture_monophoneme_I32.example.model COPYONLY)

BUILD_UNIT_TEST(TEST_SRC src/dgaussian_acoustic_model_test.cpp
                LIBS cppdecoder::Utils
                     cppdecoder::AcousticModel
                LIBS_INCLUDE ${Utils_SOURCE_DIR}/include
                             ${AcousticModel_SOURCE_DIR}/include)

BUILD_UNIT_TEST(TEST_SRC src/mixture_acoustic_model_test.cpp
                LIBS cppdecoder::Utils
                     cppdecoder::AcousticModel
                LIBS_INCLUDE ${Utils_SOURCE_DIR}/include
                             ${AcousticModel_SOURCE_DIR}/include)