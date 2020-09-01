cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(UNITTEST src/dgaussian_acoustic_model_test.cpp)
set(LIB
  cppdecoder::Utils
  cppdecoder::AcousticModel)
set(LIB_INCLUDE
  ${Utils_SOURCE_DIR}/include
  ${AcousticModel_SOURCE_DIR}/include)

BUILD_UNIT_TEST(TEST_SRC ${UNITTEST} LIBS ${LIB} LIBS_INCLUDE ${LIB_INCLUDE} "" "")