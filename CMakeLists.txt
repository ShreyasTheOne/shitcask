cmake_minimum_required(VERSION 3.10)

project(Shitcask LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(
  lib_shitcask

  src/file.cpp
  src/shitcask.cpp
  src/io_loop.cpp
)

target_include_directories(
  lib_shitcask
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# add_executable(shitcask lib_shitcask)

target_compile_features(lib_shitcask PRIVATE cxx_std_23)

add_subdirectory(tests)


