cmake_minimum_required (VERSION 2.8.11)

project(metaFloat)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -stdlib=libc++")

add_executable(metaFloat main.cpp)
