cmake_minimum_required(VERSION 3.0)
project(bitmap)
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

add_executable(bitmap_generator main.cpp fsexp.hpp fsexp.cpp)
