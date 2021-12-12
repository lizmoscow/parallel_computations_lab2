#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include "vertex.h"
#include "bfs.h"


std::vector<Vertex> buildGraph(std::string &filename, uint32_t n);
void refresh(std::vector<Vertex> &graph);
bool checkRefresh(std::vector<Vertex> &graph);


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
        std::vector<uint32_t> startPositions(repeat, dis(gen));

        auto graph = buildGraph(filename, n);

        // Sequential version
        long long int duration2 = 0;
        for (auto pos : startPositions) {
            auto startTime = std::chrono::steady_clock::now();
            bfs(graph, pos);
            auto finishTime = std::chrono::steady_clock::now();
            duration2 += std::chrono::duration_cast<std::chrono::seconds>(finishTime - startTime).count();
            if (!checkBFS(graph)) std::cout << "Some vertices have not been visited!";
            refresh(graph);
            //if (!checkRefresh(graph)) std::cout << "Some vertices have not been refreshed!";
        }
        std::cout << "Sequential BFS took " << duration2 / repeat << " seconds\n";

        // Parallel version
        long long int duration1 = 0;
        for (auto pos : startPositions) {
            auto startTime = std::chrono::steady_clock::now();
            parallelBFS(graph, pos);
            auto finishTime = std::chrono::steady_clock::now();
            duration1 += std::chrono::duration_cast<std::chrono::seconds>(finishTime - startTime).count();
            if (!checkBFS(graph)) std::cout << "Some vertices have not been visited!";
            refresh(graph);
            //if (!checkRefresh(graph)) std::cout << "Some vertices have not been refreshed!";
        }
        std::cout << "Parallel BFS took " << duration1 / repeat << " seconds\n";
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


void refresh(std::vector<Vertex> &graph) {
    for (auto &v :graph) {
        v.visited = false;
    }
}



bool checkRefresh(std::vector<Vertex> &graph) {
    bool correct = true;
    for (const auto &vertex : graph) {
        correct &= !vertex.visited;
    }
    return correct;
}