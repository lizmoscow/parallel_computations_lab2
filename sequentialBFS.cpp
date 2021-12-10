#include <iostream>
#include <vector>
#include <queue>
#include "vertex.h"

void bfs(std::vector<Vertex> &graph, uint32_t start) {
    std::queue<Vertex*> queue;
    queue.push(&graph[start]);
    graph[0].visited = true;

    while (!queue.empty()) {
        auto vertex = queue.front();
        queue.pop();
        for (auto neighbour : vertex->neighbours) {
            if (!neighbour->visited) {
                queue.push(neighbour);
                neighbour->visited = true;
            }
        }
    }
}


bool checkBFS(std::vector<Vertex> &graph) {
    bool correct = true;
    for (const auto &vertex : graph) {
        correct &= vertex.visited;
    }
    return correct;
}