cmake_minimum_required(VERSION 3.19)
project(ParallelComputationsPt2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS  --std=c++17)

add_executable(GenerateGraph generateGraph.cpp)
add_executable(BFS mainBFS.cpp bfs.cpp vertex.h bfs.h)