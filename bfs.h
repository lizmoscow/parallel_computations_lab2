//
// Created by Elizaveta Moskovskaya on 10.12.2021.
//

#ifndef PARALLELCOMPUTATIONSPT2_BFS_H
#define PARALLELCOMPUTATIONSPT2_BFS_H

void bfs(std::vector<Vertex> &graph, uint32_t start = 0);
void parallelBFS(std::vector<Vertex> &graph, uint32_t start, uint32_t* temp1, uint32_t* temp2, Vertex** queue1, Vertex** queue2);
bool checkBFS(std::vector<Vertex> &graph);

#endif //PARALLELCOMPUTATIONSPT2_BFS_H
