#include <vector>
#include <queue>
#include <numeric>
#include <execution>
#include <iostream>
#include "vertex.h"


void bfs(std::vector<Vertex> &graph, uint32_t start) {
    std::deque<Vertex*> queue;
    queue.push_back(&graph[start]);
    graph[start].visited = true;
    graph[start].distance = 0;

    while (!queue.empty()) {
        auto vertex = queue.front();
        queue.pop_front();
        for (auto neighbour : vertex->neighbours) {
            if (!neighbour->visited) {
                queue.push_back(neighbour);
                neighbour->visited = true;
                neighbour->distance = vertex->distance + 1;
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


bool checkBFSDistance(std::vector<Vertex> &graph, uint32_t n) {
    bool correct = true;
    for (const auto &vertex : graph) {
        uint32_t k = vertex.index % n;
        uint32_t j = ((vertex.index - k) % (n * n)) / n;
        uint32_t i = (vertex.index - k - j * n) / (n * n);
        if (vertex.distance != i + j + k) std::cout << vertex.distance << " " << i + j + k << "\n";
        correct &= (vertex.distance == i + j + k);
    }
    return correct;
}


void parallelBFS_stl(std::vector<Vertex> &graph, uint32_t start) {
    std::vector<Vertex*> queue1, queue2;
    queue1.reserve(graph.size());
    queue2.reserve(graph.size());
    std::vector<uint32_t> temp, range;
    temp.reserve(graph.size());
    temp.resize(1,0);
    range.resize(graph.size());
    std::iota(range.begin(), range.end(), 0);

    queue1.push_back(&graph[start]);
    graph[0].visited = true;
    graph[0].distance = 0;

    auto iteration = [&queue1, &queue2, &temp](uint32_t position)
    {
        const auto item = queue1[position];
        if (item == nullptr) return;

        auto pos = temp[position];
        for (auto neighbour : item->neighbours) {
            if (!neighbour->visited) {
                queue2[pos++] = neighbour;
                neighbour->visited = true;
                neighbour->distance = item->distance + 1;
            }
        }
    };

    while (!queue1.empty()) {
        std::transform_exclusive_scan(
                            std::execution::par,
                            queue1.begin(),
                            queue1.end(),
                            temp.begin(),
                            0,
                            std::plus<uint32_t >{},
                            [](Vertex* v) {
                                return (v == nullptr) ? 0 : v->neighbours.size();
                            });

        queue2.resize(temp.back() + 6, nullptr);

        std::for_each(
                std::execution::par,
                range.begin(),
                range.begin() + temp.size(),
                iteration);

        queue1.clear();
        std::copy_if(std::execution::par, queue2.begin(), queue2.end(),
                     std::back_inserter(queue1),
                     [](Vertex* v) { return v != nullptr; });
        queue2.clear();
        temp.resize(queue1.size(), 0);
    }
}


template<typename T>
T scan_simd(const T *array, T* out, unsigned length) {
    T scan_a = 0;
#pragma omp simd reduction(inscan,+: scan_a)
    for(uint32_t i = 0; i < length; i++) {
        out[i] = scan_a;
#pragma omp scan exclusive(scan_a)
        scan_a += array[i];
    }
    return scan_a;
}

template<typename T, typename P>
void map(T* in, P* out, uint32_t length, auto &&predicate) {
    #pragma omp parallel for
    for (uint32_t i = 0; i < length; ++i) {
        out[i] = predicate(in[i]);
    }
}

template<typename T>
uint32_t filter(T* in, T* out, uint32_t* temp1, uint32_t* temp2, uint32_t length, auto &&predicate) {
    map(in, temp1, length, predicate);
    auto size = scan_simd(temp1, temp2, length);
    #pragma omp parallel for
    for (uint32_t i = 0; i < length; ++i) {
        if (temp1[i]) {
            out[temp2[i]] = in[i];
        }
    }
    return size;
}

void parallelBFS(std::vector<Vertex> &graph, uint32_t start,
                     uint32_t* temp1, uint32_t* temp2,
                     Vertex** queue1, Vertex** queue2) {
    uint32_t sizeQueue1 = 1;

    queue1[0] = &graph[start];
    queue1[0]->visited = true;
    queue1[0]->distance = 0;

    auto pred1 = [](Vertex* v){
        return v->neighbours.size();
    };
    auto pred2 = [](Vertex* v)->uint32_t {
        return v != nullptr;
    };

    while (sizeQueue1 > 0) {
        map(queue1, temp1, sizeQueue1, pred1);
        auto size = scan_simd(temp1, temp2, sizeQueue1);

        #pragma omp parallel for
            for (uint32_t i = 0; i < sizeQueue1; ++i) {
                const auto item = queue1[i];

                auto pos = temp2[i];
                for (auto neighbour : item->neighbours) {
                    bool visited;
                    #pragma omp atomic capture
                    {
                        visited = neighbour->visited;
                        neighbour->visited = true;
                    }
                    queue2[pos++] = (!visited) ? neighbour : nullptr;
                    neighbour->distance = (!visited) ? item->distance + 1 : neighbour->distance;
                }
            }
        sizeQueue1 = filter(queue2, queue1,
                            temp1, temp2,
                            size,
                            pred2);
    }
}