#ifndef PARALLELCOMPUTATIONSPT2_VERTEX_H
#define PARALLELCOMPUTATIONSPT2_VERTEX_H

class Vertex {
public:
    uint32_t index;
    std::vector<Vertex*> neighbours;
    bool visited = false;
    Vertex(uint32_t i, std::vector<Vertex*> &n): index(i), neighbours(std::move(n)) { };
    explicit Vertex(uint32_t i) : index(i) { neighbours.reserve(6); };
};

#endif //PARALLELCOMPUTATIONSPT2_VERTEX_H
