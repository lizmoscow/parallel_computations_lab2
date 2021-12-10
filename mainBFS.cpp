#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include "vertex.h"
#include "sequentialBFS.h"


std::vector<Vertex> buildGraph(std::string &filename, uint32_t n);


int main(int argc, char** argv) {
    try {
        if (argc != 4){
            throw std::runtime_error("Wrong arguments!");
        }
        std::string filename = argv[1];
        int32_t n = std::stoi(argv[2]);
        int32_t repeat = std::stoi(argv[3]);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, std::pow(n, 3));

        auto graph = buildGraph(filename, n);

        auto startTime = std::chrono::system_clock::now();
        for (uint32_t i = 0; i < repeat; ++ i) {
            bfs(graph, dis(gen));
        }
        auto finishTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - startTime).count();
        std::cout << "BFS took " << duration / repeat << " milliseconds\n";

        if (!checkBFS(graph)) {
            std::cout << "Some vertices have not been visited!";
        }
        else {
            std::cout << "Algorithm works correctly!";
        }
    }
    catch (std::exception &e) {
        std::cerr << e.what();
    }
    return 0;
}


std::vector<Vertex> buildGraph(std::string &filename, uint32_t n) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open the file!");
    }

    std::vector<Vertex> graph;
    graph.reserve(std::pow(n, 3));
    for (uint32_t i = 0; i < std::pow(n, 3); ++i) {
        graph.emplace_back(Vertex(i));
    }

    uint32_t index, numberOfNeighbours;
    while (in >> index >> numberOfNeighbours) {
        std::vector<uint32_t> neighbours;
        neighbours.reserve(6);
        uint32_t nghbr;
        for (uint32_t i = 0; i < numberOfNeighbours; ++i) {
            in >> nghbr;
            graph[index].neighbours.push_back(&graph[nghbr]);
        }
    }

    in.close();
    return graph;
}


