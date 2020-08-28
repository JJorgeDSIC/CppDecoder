cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(NAME Sample)
set(REPO_VERSION 0.0.1)

project(${NAME} VERSION ${REPO_VERSION} DESCRIPTION "Sample")

set(CMAKE_C_STANDARD 99)

# Check and add the C++ version.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX14)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++0x and higher support. Please use a different C++ compiler.")
endif()

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
  src/Frame.cpp
  src/Sample.cpp)

set(HEADER_PATHS include)
set(HEADER_FILES
  include/Frame.h
  include/Sample.h)

include_directories(
  ${HEADER_PATHS}
  ${utils_SOURCE_DIR}/include)

add_executable(${NAME} ${SOURCE_FILES})
add_executable(cppdecoder::Sample ALIAS ${NAME})
add_executable(cppdecoder::Frame ALIAS ${NAME})

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