cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

BUILD_UNIT_TEST(TEST_SRC src/sample_test.cpp
                LIBS cppdecoder::Utils
                     cppdecoder::Sample
                LIBS_INCLUDE ${Utils_SOURCE_DIR}/include
                             ${Sample_SOURCE_DIR}/include)

BUILD_UNIT_TEST(TEST_SRC src/frame_test.cpp
               LIBS cppdecoder::Utils
                    cppdecoder::Sample
               LIBS_INCLUDE ${Utils_SOURCE_DIR}/include
                    ${Sample_SOURCE_DIR}/include)
