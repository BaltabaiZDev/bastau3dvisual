#pragma once
#include "node.cpp"

struct Edge {
    int a, b;
    float weight;
    Edge(int a_, int b_, float w=1.0f): a(a_), b(b_), weight(w) {}
};
