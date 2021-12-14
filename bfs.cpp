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
    uint32_t neighbourCount;

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

    /*auto initTemp = [&temp, &neighbourCount](Vertex* v){
        if (v == nullptr) return;
        temp.emplace_back(std::make_tuple(neighbourCount, v));
        neighbourCount += v->neighbours.size();
    };*/

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

        neighbourCount = 0;
        for (auto q : queue1) {
            if (q == nullptr) continue;
            temp.emplace_back(std::make_tuple(neighbourCount, q));
            neighbourCount += q->neighbours.size();
        }

        if (temp.empty()) return;
        queue2.resize(neighbourCount, nullptr);

        if (temp.size() > 1000) {
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
        /*if (temp.size() > 100) {
            #pragma omp parallel for
            {
                for (auto t : temp) {
                    iteration(t);
                }
            }
        }
        else {
            for (auto t : temp) {
                iteration(t);
            }
        }*/

        queue1.swap(queue2);
        queue2.clear();
        temp.clear();
    }
}


void bfs(std::vector<Vertex> &graph, uint32_t start) {
    std::deque<Vertex*> queue;
    queue.push_back(&graph[start]);
    graph[0].visited = true;

    while (!queue.empty()) {
        auto vertex = queue.front();
        queue.pop_front();
        for (auto neighbour : vertex->neighbours) {
            if (!neighbour->visited) {
                queue.push_back(neighbour);
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


void parallelBFS_s(std::vector<Vertex> &graph, uint32_t start,
                   Vertex** queue1, Vertex** queue2, std::tuple<uint32_t, Vertex*>* temp) {
    queue1[0] = &graph[start];
    queue1[0]->visited = true;
    uint32_t sizeQueue1 = 1;

    while (sizeQueue1 > 0) {
        uint32_t neighbourCount = 0, sizeTemp = 0;
        for (uint32_t i = 0; i < sizeQueue1; ++i) {
            if (queue1[i] == nullptr) continue;
            temp[sizeTemp++] = std::make_tuple(neighbourCount, queue1[i]);
            neighbourCount += queue1[i]->neighbours.size();
        }
        //std::cout << "Temp initialized\n";

        if (sizeTemp == 0) return;
#pragma omp parallel for
        {
            for (uint32_t i = 0; i < sizeTemp; ++i) {
                auto item = std::get<1>(temp[i]);
                if (item == nullptr) return;

                auto pos = std::get<0>(temp[i]);
                for (auto neighbour : item->neighbours) {
                    if (!neighbour->visited) {
                        queue2[pos++] = neighbour;
                        neighbour->visited = true;
                    }
                }
            }
        }
        std::swap(queue1, queue2);
        for (uint32_t i = 0; i < sizeQueue1; ++i) {
            queue2[i] = nullptr;
        }
        sizeQueue1 = neighbourCount;
        //std::cout << "Queue updated\n";
    }
}


void parallelBFS_new(std::vector<Vertex> &graph, uint32_t start) {
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

    auto iteration = [&queue1, &queue2, &temp](uint32_t position)
    {
        const auto item = queue1[position];
        if (item == nullptr) return;

        auto pos = temp[position];
        for (auto neighbour : item->neighbours) {
            if (!neighbour->visited) {
                queue2[pos++] = neighbour;
                neighbour->visited = true;
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


/*void parallelBFS_omp(std::vector<Vertex> &graph, uint32_t start,
                     uint32_t* temp1, uint32_t* temp2,
                     Vertex** queue1, Vertex** queue2) {
    uint32_t sizeQueue1 = 1;

    queue1[0] = &graph[start];
    queue1[0]->visited = true;

    while (sizeQueue1 > 0) {

        #pragma omp parallel for
        {
            for (uint32_t i = 0; i < sizeQueue1; ++i) {
                temp1[i] = (queue1[i] == nullptr) ? 0 : queue1[i]->neighbours.size();
            }
        }
        auto size = scan_simd(temp1, temp2, sizeQueue1);
        if (size == 0) return;

        #pragma omp parallel for
        {
            for (uint32_t i = 0; i < sizeQueue1; ++i) {
                const auto item = queue1[i];
                if (item == nullptr) continue;

                auto pos = temp2[i];
                for (auto neighbour : item->neighbours) {
                    bool visited;
                    #pragma omp atomic capture
                    {
                        visited = neighbour->visited;
                        neighbour->visited = true;
                    }
                    queue2[pos++] = (!visited) ? neighbour : nullptr;
                }
            }
        }
        std::swap(queue1, queue2);
        sizeQueue1 = size;
    }
}*/


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

void parallelBFS_omp_wcopy(std::vector<Vertex> &graph, uint32_t start,
                     uint32_t* temp1, uint32_t* temp2,
                     Vertex** queue1, Vertex** queue2) {
    uint32_t sizeQueue1 = 1;

    queue1[0] = &graph[start];
    queue1[0]->visited = true;

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
                }
            }
        sizeQueue1 = filter(queue2, queue1,
                            temp1, temp2,
                            size,
                            pred2);
    }
}