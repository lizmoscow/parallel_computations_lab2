#include <iostream>
#include <fstream>
#include <vector>

void generateGraph(std::string &filename, int32_t n);

int main(int argc, char** argv) {
    try {
        if (argc != 3){
            throw std::runtime_error("Wrong arguments!");
        }
        std::string filename = argv[1];
        int32_t n = std::stoi(argv[2]);
        generateGraph(filename, n);
    }
    catch (std::exception &e) {
        std::cerr << e.what();
    }
    return 0;
}


void generateGraph(std::string &filename, int32_t n) {
    std::ofstream out(filename, std::ios::app);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open the file!");
    }
    auto neighbours = std::vector<int32_t>(6);
    for (int32_t i = 0; i < n; ++i) {
        for (int32_t j = 0; j < n; ++j) {
            for (int32_t k = 0; k < n; ++k) {
                auto position = i * n * n + j * n + k;
                neighbours.clear();
                if (i < n - 1) {
                    neighbours.push_back((i + 1) * n * n + j * n + k);
                }
                if (i > 0) {
                    neighbours.push_back((i - 1) * n * n + j * n + k);
                }
                if (j < n - 1) {
                    neighbours.push_back(i * n * n + (j + 1) * n + k);
                }
                if (j > 0) {
                    neighbours.push_back(i * n * n + (j - 1) * n + k);
                }
                if (k < n - 1) {
                    neighbours.push_back(i * n * n + j * n + k + 1);
                }
                if (k > 0) {
                    neighbours.push_back(i * n * n + j * n + k - 1);
                }
                out << position << " " << neighbours.size() << " ";
                for (int32_t el : neighbours) {
                    out << el << " ";
                }
                out << "\n";
            }
        }
    }
    out.close();
}