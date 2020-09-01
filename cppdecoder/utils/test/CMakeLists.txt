cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(UNITTEST src/utils_test.cpp)
set(LIB cppdecoder::Utils)
set(LIB_INCLUDE ${Utils_SOURCE_DIR}/include)

BUILD_UNIT_TEST(TEST_SRC ${UNITTEST} LIBS ${LIB} LIBS_INCLUDE ${LIB_INCLUDE})