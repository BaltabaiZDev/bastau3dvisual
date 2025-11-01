#pragma once
#include <glm/glm.hpp>

enum class NodeState { Neutral, Pending, Done, Fail };

struct Node {
    int id;
    glm::vec3 pos;        // current
    glm::vec3 basePos;    // roam center
    glm::vec3 vel;        // velocity
    float roamRadius = 0.12f;
    NodeState state = NodeState::Neutral;
};
