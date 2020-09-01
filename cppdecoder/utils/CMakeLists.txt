cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(NAME Utils)
set(REPO_VERSION 0.0.1)

project(${NAME} VERSION ${REPO_VERSION} DESCRIPTION "Decoder utils")

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
  src/Utils.cpp)

set(HEADER_PATHS include)
set(HEADER_FILES
  include/Utils.h)

include_directories(
  ${HEADER_PATHS})

add_library(${NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
add_library(cppdecoder::Utils ALIAS ${NAME})

# Set the debug or relese mode.
if (CMAKE_BUILD_TYPE MATCHES Debug)
  # Debug level
  add_definitions(-DDEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
  # Optimization level
else ()
  message(FATAL_ERROR "Set the build type with -DCMAKE_BUILD_TYPE=<type>")
endif()

include(GenerateExportHeader)
generate_export_header(${NAME})

target_include_directories(${NAME}
  PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})