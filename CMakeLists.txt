cmake_minimum_required(VERSION 3.10)

# Project name and version
project(static_vector VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Disable warnings as errors for external projects
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")

# Specifically disable the uninitialized variable warning
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")

# Add GoogleTest
include(cmake/googletest.cmake)
enable_testing()
add_subdirectory(tests)

# Add Google benchmark
include(cmake/googlebenchmark.cmake)
add_subdirectory(benchmarks)