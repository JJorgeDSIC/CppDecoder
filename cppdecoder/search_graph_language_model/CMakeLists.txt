cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(NAME SearchGraphLanguageModel)
set(REPO_VERSION 0.0.1)

project(${NAME} VERSION ${REPO_VERSION} DESCRIPTION "Search Graph Language Model")

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# Set the debug or relese mode.
if (CMAKE_BUILD_TYPE MATCHES Debug)
  # Debug level
  add_definitions(-DDEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
  # Optimization level
else ()
  message(FATAL_ERROR "Set the build type with -DCMAKE_BUILD_TYPE=<type>")
endif()

set(SOURCE_FILES
  src/SearchGraphLanguageModel.cpp)

set(HEADER_PATHS include)
set(HEADER_FILES
  include/SearchGraphLanguageModel.h)

include_directories(
  ${HEADER_PATHS}
  ${Utils_SOURCE_DIR}/include)

configure_file(
  ${CMAKE_SOURCE_DIR}/models/2.gram.graph ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/models/2.gram.graph COPYONLY)

link_directories("${CMAKE_BINARY_DIR}/bin/")

add_executable(${NAME} ${SOURCE_FILES})
add_executable(cppdecoder::SearchGraphLanguageModel ALIAS ${NAME})

# Set the debug or relese mode.
if (CMAKE_BUILD_TYPE MATCHES Debug)
  # Debug level
  add_definitions(-DDEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
  # Optimization level
else ()
  message(FATAL_ERROR "Set the build type with -DCMAKE_BUILD_TYPE=<type>")
endif()

target_include_directories(${NAME}
  PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})

target_link_libraries(${NAME}
  cppdecoder::Utils)