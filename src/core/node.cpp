#pragma once
#include <glm/glm.hpp>
#include <string>

struct Node {
    int id;
    glm::vec3 pos;
    float size;
    glm::vec3 color;
    float weight;

    Node(int id_, const glm::vec3& p, float s=0.06f, const glm::vec3& c={0.2f,0.7f,1.0f}, float w=1.0f)
        : id(id_), pos(p), size(s), color(c), weight(w) {}
};
