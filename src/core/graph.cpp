#include "graph.h"
#include <glm/glm.hpp>
#include <random>
#include <cmath>
#include <algorithm>

static std::mt19937& rng() {
    static thread_local std::mt19937 gen{ std::random_device{}() };
    return gen;
}

// Сфера ішіндегі нүкте (көлем бойынша біркелкі)
static glm::vec3 randomInSphere(float R) {
    std::uniform_real_distribution<float> U(0.0f, 1.0f);
    float u = U(rng());
    float v = U(rng());
    float w = U(rng());

    float theta = 2.0f * 3.14159265358979323846f * v;
    float z = 2.0f * w - 1.0f;
    float r = std::sqrt(std::max(0.0f, 1.0f - z*z));
    float rad = R * std::cbrt(u);

    return rad * glm::vec3(r*std::cos(theta), z, r*std::sin(theta));
}

// ✅ member ретінде дәл анықталады
Node Graph::makeRandomNode() {
    Node nd{};
    nd.id  = nextId++;
    nd.pos = glm::vec3(0.0f);
    nd.vel = glm::vec3(0.0f);
    nd.roamRadius = 0.12f;
    nd.state = NodeState::Pending;

    int n = std::max(1, (int)nodes.size());
    float worldR = std::max(1.2f, 0.28f * std::cbrt((float)n));
    nd.basePos = randomInSphere(worldR);
    nd.pos     = nd.basePos;
    return nd;
}

Graph::Graph(int initialCount) {
    nodes.reserve(initialCount);
    for (int i = 0; i < initialCount; ++i) {
        Node nd = makeRandomNode();
        idIndex[nd.id] = nodes.size();
        nodes.push_back(nd);
    }
    rebuildRingEdges();
}

int Graph::addTask() {
    Node nd = makeRandomNode();
    idIndex[nd.id] = nodes.size();
    nodes.push_back(nd);
    rebuildRingEdges();
    return nd.id;
}

bool Graph::removeTask(int id) {
    auto it = idIndex.find(id);
    if (it == idIndex.end()) return false;

    size_t idx = it->second;
    size_t last = nodes.size() - 1;

    if (idx != last) {
        std::swap(nodes[idx], nodes[last]);
        idIndex[nodes[idx].id] = idx;
    }
    nodes.pop_back();
    idIndex.erase(it);
    rebuildRingEdges();
    return true;
}

void Graph::setNodeState(int id, NodeState s) {
    auto it = idIndex.find(id);
    if (it == idIndex.end()) return;
    nodes[it->second].state = s;
}

std::vector<int> Graph::ids() const {
    std::vector<int> out;
    out.reserve(nodes.size());
    for (const auto& n : nodes) out.push_back(n.id);
    std::sort(out.begin(), out.end());
    return out;
}

void Graph::rebuildRingEdges() {
    edges.clear();
    int n = (int)nodes.size();
    if (n < 2) return;
    edges.reserve(n);
    for (int i = 0; i < n; ++i) {
        edges.push_back({ i, (i + 1) % n });
    }
}

void Graph::update(float dt) {
    if (dt <= 0.0f) return;

    const int n = (int)nodes.size();
    if (n == 0) return;

    const float worldR = std::max(1.2f, 0.28f * std::cbrt((float)n));
    const float B = worldR + 0.6f;
    const float maxSpeed = 0.7f;

    const float nodeRadius = 0.07f;
    const float minGap     = 0.03f;
    const float minDist    = 2.0f * nodeRadius + minGap;
    const float minDist2   = minDist * minDist;
    const float sepK       = 10.0f;

    const float jitterScale = 0.6f;
    const float springNear  = 0.5f;
    const float springFar   = 2.0f;

    std::uniform_real_distribution<float> J(-1.0f, 1.0f);

    // Жұптық сепарация жинақтағышы
    std::vector<glm::vec3> sepAcc(n, glm::vec3(0.0f));
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            glm::vec3 d = nodes[i].pos - nodes[j].pos;
            float dist2 = glm::dot(d, d);
            if (dist2 > 1e-10f && dist2 < minDist2) {
                float dist = std::sqrt(dist2);
                glm::vec3 dir = d / dist;
                float overlap = (minDist - dist);
                glm::vec3 acc = dir * (sepK * overlap);
                sepAcc[i] += acc;
                sepAcc[j] -= acc;
            }
        }
    }

    // Интеграция
    for (int i = 0; i < n; ++i) {
        auto& nd = nodes[i];

        glm::vec3 a(J(rng()), J(rng()), J(rng()));
        a *= jitterScale;

        glm::vec3 toC = nd.basePos - nd.pos;
        float d = glm::length(toC);
        if (d > 1e-5f) {
            glm::vec3 dir = toC / d;
            float springK = (d > nd.roamRadius) ? springFar : springNear;
            a += dir * springK;
        }

        switch (nd.state) {
            case NodeState::Pending: a += glm::vec3(0.f,  0.20f, 0.f); break;
            case NodeState::Done:    a += glm::vec3(0.f,  0.35f, 0.f); break;
            case NodeState::Fail:    a += glm::vec3(0.f, -0.30f, 0.f); break;
            default: break;
        }

        a += sepAcc[i];

        nd.vel += a * dt;
        float sp = glm::length(nd.vel);
        if (sp > maxSpeed) nd.vel = (nd.vel / sp) * maxSpeed;
        nd.pos += nd.vel * dt;

        for (int ax = 0; ax < 3; ++ax) {
            float* p = (ax==0)? &nd.pos.x : (ax==1)? &nd.pos.y : &nd.pos.z;
            float* v = (ax==0)? &nd.vel.x : (ax==1)? &nd.vel.y : &nd.vel.z;
            if (*p < -B) { *p = -B; *v = std::abs(*v);  }
            if (*p >  +B) { *p =  +B; *v = -std::abs(*v); }
        }
    }
}
