cmake_minimum_required(VERSION 3.19)
project(ParallelComputationsPt2)

set(CMAKE_CXX_STANDARD 14)

add_executable(GenerateGraph generateGraph.cpp)
add_executable(BFS mainBFS.cpp sequentialBFS.cpp vertex.h sequentialBFS.h)