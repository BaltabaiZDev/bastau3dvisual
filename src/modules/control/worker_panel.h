#pragma once
#include "imgui.h"
#include "modules/control/message_bus.h"

struct WorkerPanel {
    MessageBus& bus;
    int myId = 0;

    void draw() {
        ImGui::Text("Worker panel");
        ImGui::Separator();
        ImGui::InputInt("My node id", &myId);
        if (myId < 0) myId = 0;

        if (ImGui::Button("Done")) bus.markDone(myId);
        ImGui::SameLine();
        if (ImGui::Button("Fail")) bus.markFail(myId);
    }
};
