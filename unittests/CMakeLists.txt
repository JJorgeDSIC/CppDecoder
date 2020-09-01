cmake_minimum_required(VERSION 3.3)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

function(build_unit_test UNIT_TEST LIBS LIBS_INCLUDE EXT_SOURCE EXT_INCLUDE)
  # I used a simple string replace, to cut off .cpp.
  string(REPLACE "src/" "" UNIT_TEST_NAME ${UNIT_TEST})
  string(REPLACE ".cpp" "" UNIT_TEST_NAME ${UNIT_TEST_NAME})
  string(REPLACE "_test" "" UNIT_TEST_NAME_TEST ${UNIT_TEST_NAME})

  add_executable(${UNIT_TEST_NAME} ${UNIT_TEST} ${EXT_SOURCE})

  # Bring the headers into the project
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LIBS_INCLUDE}
    ${EXT_INCLUDE})

  target_link_libraries(${UNIT_TEST_NAME}
    gtest_main
    ${LIBS})

  add_test(NAME ${UNIT_TEST_NAME_TEST} COMMAND ${UNIT_TEST_NAME})
endfunction()
