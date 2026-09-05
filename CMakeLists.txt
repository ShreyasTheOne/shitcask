cmake_minimum_required(VERSION 3.10)

project(Shitcask LANGUAGES CXX)

add_subdirectory(src)

target_compile_features(shitcask PRIVATE cxx_std_23)

