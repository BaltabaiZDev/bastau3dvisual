#pragma once
#include "node.h"
#include "edge.h"
#include <vector>
#include <unordered_map>

class Graph {
    // --- Storage ---
    std::vector<Node> nodes;                    // contiguous for renderer
    std::unordered_map<int, size_t> idIndex;    // id -> index in nodes
    std::vector<Edge> edges;                    // ✅ edges контейнері
    int nextId = 0;

    // Жаңа түйінге еркін basePos беру
    Node makeRandomNode();

public:
    explicit Graph(int initialCount = 0);

    // Басқару API
    int  addTask();                 // жаңа Node (Pending күйімен)
    bool removeTask(int id);        // id бойынша жою

    // Көмекші
    bool has(int id) const { return idIndex.count(id) != 0; }
    std::vector<int> ids() const;   // барлық қолданыстағы ID

    // Күй өзгерту
    void setNodeState(int id, NodeState state);

    // Кадр жаңарту
    void update(float dt);

    // --- Accessors ---
    std::vector<Node>&       getNodes()       { return nodes; }
    const std::vector<Node>& getNodes() const { return nodes; }

    // ✅ Edges аксессорлары (әзірге бос болуы мүмкін — сызу үшін жеткілікті)
    std::vector<Edge>&             getEdges()       { return edges; }
    const std::vector<Edge>&       getEdges() const { return edges; }

    int  count() const { return (int)nodes.size(); }
};
