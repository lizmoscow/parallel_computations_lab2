#include <iostream>
#include <vector>
#include <queue>
#include <numeric>
#include <algorithm>
#include <execution>
#include "vertex.h"


void parallelBFS(std::vector<Vertex> &graph, uint32_t start) {
    std::vector<Vertex*> queue1, queue2;
    queue1.reserve(graph.size());
    queue2.reserve(graph.size());
    std::vector<std::tuple<uint32_t, Vertex*>> temp;
    temp.reserve(graph.size());
    queue1.push_back(&graph[start]);
    graph[0].visited = true;

    auto iteration = [&queue2](std::tuple<uint32_t, Vertex*>& positions)
    {
        auto item = std::get<1>(positions);
        if (item == nullptr) return;

        auto pos = std::get<0>(positions);
        for (auto neighbour : item->neighbours) {
            if (!neighbour->visited) {
                queue2[pos++] = neighbour;
                neighbour->visited = true;
            }
        }
    };

    while (!queue1.empty()) {

        /*for (auto q : queue1) {
            if (q == nullptr) continue;
            temp.emplace_back(std::make_tuple(q->neighbours.size(), 0));
        }
        std::exclusive_scan(//std::execution::par,
                            temp.begin(),
                            temp.end(),
                            temp.begin(),
                            std::make_tuple(0, 0),
                            [](std::tuple<uint32_t, uint32_t> a, std::tuple<uint32_t, uint32_t> b) {
                                return std::make_tuple(std::get<0>(a) + std::get<0>(b), 0);
                            });
        auto count = 0;
        for (uint32_t i = 0; i < queue1.size(); ++i) {
            if (queue1[i] == nullptr) continue;
            temp[count] = std::make_tuple(std::get<0>(temp[count]), i);
            count++;
        }*/

        uint32_t neighbourCount = 0;
        for (auto q : queue1) {
            if (q == nullptr) continue;
            temp.emplace_back(std::make_tuple(neighbourCount, q));
            neighbourCount += q->neighbours.size();
        }

        if (temp.empty()) return;
        queue2.resize(std::get<0>(temp.back())
                        + std::get<1>(temp.back())->neighbours.size(), nullptr);

        if (temp.size() > 500) {
            std::for_each(
                    std::execution::par,
                    temp.begin(),
                    temp.end(),
                    iteration);
        } else {
            std::for_each(
                    std::execution::seq,
                    temp.begin(),
                    temp.end(),
                    iteration);
        }
        queue1.swap(queue2);
        queue2.clear();
        temp.clear();
    }
}


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