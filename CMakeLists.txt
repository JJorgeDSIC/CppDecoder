cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(CppDecoder DESCRIPTION "C++ Decoder")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

enable_testing()
include(CTest)
add_subdirectory(unittests)

add_subdirectory(cppdecoder/utils)
add_subdirectory(cppdecoder/sample)
add_subdirectory(cppdecoder/acoustic_model)
add_subdirectory(cppdecoder/search_graph_language_model)
add_subdirectory(cppdecoder/)
