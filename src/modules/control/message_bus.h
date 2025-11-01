#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "core/graph.h"

struct MessageBus {
    Graph* g = nullptr;
    explicit MessageBus(Graph& ref) : g(&ref) {}

    int nodeCount() const noexcept { return g ? g->count() : 0; }

    // ✅ Контроллер тек тапсырма береді/жояды
    int  addTask()                { return g ? g->addTask() : -1; }
    bool removeTask(int id)       { return g ? g->removeTask(id) : false; }

    // ✅ Worker панелінде қолдануға қалады (Done/Fail)
    void markDone(int id)         { if (g) g->setNodeState(id, NodeState::Done); }
    void markFail(int id)         { if (g) g->setNodeState(id, NodeState::Fail); }

    // Қолайлық үшін ID-лер тізімі (UI-ға пайдалы болуы мүмкін)
    std::vector<int> ids()  const { return g ? g->ids() : std::vector<int>{}; }
};
