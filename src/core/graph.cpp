#pragma once
#include <vector>
#include <random>
#include "node.cpp"
#include "edge.cpp"

struct Graph {
    std::vector<Node> nodes;
    std::vector<Edge> edges;

    void makeDemo(int n=48, unsigned seed=42) {
        nodes.clear(); edges.clear();
        std::mt19937 rng(seed);
        std::uniform_real_distribution<float> U(-1.0f, 1.0f);
        std::uniform_real_distribution<float> C(0.2f, 1.0f);

        for (int i=0;i<n;++i) {
            glm::vec3 p(U(rng), U(rng)*0.6f, U(rng));
            glm::vec3 col(C(rng), C(rng), C(rng));
            nodes.emplace_back(i, p, 0.045f, col, C(rng));
        }
        // Жеңіл “жақынға қосу”
        for (int i=0;i<n;++i) {
            for (int j=i+1;j<n;++j) {
                float d = glm::length(nodes[i].pos - nodes[j].pos);
                if (d < 0.65f && (i+j)%7==0) edges.emplace_back(i,j, 1.0f/d);
            }
        }
    }
};
