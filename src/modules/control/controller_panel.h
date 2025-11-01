#pragma once
#include "imgui.h"
#include "modules/control/message_bus.h"

struct ControllerPanel {
    MessageBus& bus;
    int addCount = 1;     // бірден бірнеше қосу үшін
    int removeId = 0;     // нақты id-мен жою

    void draw() {
        ImGui::Text("Total tasks: %d", bus.nodeCount());
        ImGui::Separator();

        // --- Add ---
        ImGui::InputInt("Add count", &addCount);
        if (addCount < 1) addCount = 1;
        if (ImGui::Button("Add task(s)")) {
            for (int i = 0; i < addCount; ++i) {
                bus.addTask();
            }
        }

        ImGui::Separator();

        // --- Remove by id ---
        ImGui::InputInt("Remove task id", &removeId);
        if (removeId < 0) removeId = 0;
        if (ImGui::Button("Remove by id")) {
            bus.removeTask(removeId);
        }

        // Қаласаңыз, ID тізімін көрсетіп қоюға болады:
        if (ImGui::CollapsingHeader("Existing IDs", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto v = bus.ids();
            ImGui::Text("Count: %d", (int)v.size());
            ImGui::BeginChild("ids_scroller", ImVec2(0, 120), true);
            int col = 0;
            for (int id : v) {
                ImGui::Text("%d", id);
                if (++col % 8 != 0) ImGui::SameLine();
            }
            ImGui::EndChild();
        }
    }
};
