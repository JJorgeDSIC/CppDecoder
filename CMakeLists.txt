cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

add_subdirectory(cppdecoder/utils)
add_subdirectory(cppdecoder/d_gaussian_acustic_model)
add_subdirectory(cppdecoder/mixture_acoustic_model)
add_subdirectory(cppdecoder/tied_states_acoustic_model)
add_subdirectory(cppdecoder/search_graph_language_model)
add_subdirectory(cppdecoder/feature)