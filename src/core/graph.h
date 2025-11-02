#pragma once
#include "node.h"
#include "edge.h"
#include <vector>
#include <unordered_map>

class Graph {
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::unordered_map<int, size_t> idIndex;
    int nextId = 0;

    Node makeRandomNode();              // ✅ private member

public:
    explicit Graph(int initialCount = 0);

    // Басқару
    int  addTask();
    bool removeTask(int id);

    // Күй
    void setNodeState(int id, NodeState state);

    // Кадр сайын жаңарту
    void update(float dt);              // ✅ дәл осы сигнатура

    // Көмекші/рендерге
    const std::vector<Edge>& getEdges() const { return edges; }
    std::vector<Node>&       getNodes()       { return nodes; }
    const std::vector<Node>& getNodes() const { return nodes; }
    int  count() const { return (int)nodes.size(); }
    std::vector<int> ids() const;       // ✅ UI үшін

    // Демонстрация үшін қарапайым ребра генерациясы
    void rebuildRingEdges();
};
