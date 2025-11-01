#pragma once
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../core/graph.h"
#include "../utils/camera.h"
#include "../ui/theme.h"

// World→Screen проекциясы (көрінсе true)
inline bool worldToScreen(const glm::vec3& P, const Camera3D& cam, int w, int h, ImVec2& out) {
    glm::mat4 PV = cam.proj(w,h) * cam.view();
    glm::vec4 clip = PV * glm::vec4(P, 1.0f);
    if (clip.w <= 0.0f) return false;
    glm::vec3 ndc = glm::vec3(clip) / clip.w; // [-1,1]
    if (ndc.x < -1 || ndc.x > 1 || ndc.y < -1 || ndc.y > 1 || ndc.z < -1 || ndc.z > 1) return false;
    float sx = (ndc.x * 0.5f + 0.5f) * (float)w;
    float sy = (1.0f - (ndc.y * 0.5f + 0.5f)) * (float)h;
    out = ImVec2(sx, sy);
    return true;
}

inline void drawLegendAndStats(const Graph& g) {
    ImGui::Begin("Legend / Stats", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
    ImGui::Text("Nodes: %d", g.count());
    ImGui::Separator();
    ImGui::TextColored(ImVec4(Theme::N_PEN[0], Theme::N_PEN[1], Theme::N_PEN[2],1),"Pending");
    ImGui::TextColored(ImVec4(Theme::N_DON[0], Theme::N_DON[1], Theme::N_DON[2],1),"Done");
    ImGui::TextColored(ImVec4(Theme::N_FAI[0], Theme::N_FAI[1], Theme::N_FAI[2],1),"Fail");
    ImGui::Separator();
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::End();
}

inline void drawLabelsOverlay(const Graph& g, const Camera3D& cam, int w, int h, int hoveredId) {
    auto* draw = ImGui::GetForegroundDrawList();
    for (const auto& n : g.getNodes()) {
        ImVec2 pt;
        if (!worldToScreen(n.pos, cam, w, h, pt)) continue;
        // Ховер болса – ашықтау фон
        ImU32 bg = (n.id==hoveredId) ? Theme::colU32(1,1,0.2f,0.25f) : Theme::colU32(0,0,0,0.35f);
        ImU32 fg = IM_COL32_WHITE;
        const char* stateTxt =
            (n.state==NodeState::Pending)?"Pending":
            (n.state==NodeState::Done)   ?"Done":
            (n.state==NodeState::Fail)   ?"Fail":"Neutral";
        char buf[64];
        snprintf(buf, sizeof(buf), "#%d  %s", n.id, stateTxt);
        ImVec2 sz = ImGui::CalcTextSize(buf);
        ImVec2 pad(6,3);
        ImVec2 p0(pt.x - sz.x*0.5f - pad.x, pt.y - 18 - pad.y);
        ImVec2 p1(pt.x + sz.x*0.5f + pad.x, pt.y - 18 + sz.y + pad.y);
        draw->AddRectFilled(p0, p1, bg, 6.0f);
        draw->AddText(ImVec2(p0.x+pad.x, p0.y+pad.y), fg, buf);
    }
}
