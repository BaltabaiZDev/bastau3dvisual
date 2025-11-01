#pragma once
#include <imgui.h>

// ImGui және OpenGL материал түстері (state → color)
namespace Theme {
    // ImGui color helpers
    inline ImU32 colU32(float r, float g, float b, float a=1.0f) {
        return IM_COL32((int)(r*255),(int)(g*255),(int)(b*255),(int)(a*255));
    }

    // Legend colors
    static constexpr float N_NEU[3] = {0.80f, 0.80f, 0.80f};
    static constexpr float N_PEN[3] = {1.00f, 1.00f, 0.00f};
    static constexpr float N_DON[3] = {0.00f, 1.00f, 0.00f};
    static constexpr float N_FAI[3] = {1.00f, 0.00f, 0.00f};

    // Material intensities
    static constexpr float DIF[4] = {0.90f, 0.90f, 0.92f, 1.0f};
    static constexpr float EM0[4] = {0.00f, 0.00f, 0.00f, 1.0f};
    static constexpr float AMB[4] = {0.20f, 0.20f, 0.22f, 1.0f}; // сәл көтеріңкі
    static constexpr float SPC[4] = {0.18f, 0.18f, 0.18f, 1.0f};

    inline void setMaterialByState(NodeState st) {
        float dif[4];
        switch (st) {
            case NodeState::Neutral: dif[0]=N_NEU[0]; dif[1]=N_NEU[1]; dif[2]=N_NEU[2]; break;
            case NodeState::Pending: dif[0]=N_PEN[0]; dif[1]=N_PEN[1]; dif[2]=N_PEN[2]; break;
            case NodeState::Done:    dif[0]=N_DON[0]; dif[1]=N_DON[1]; dif[2]=N_DON[2]; break;
            case NodeState::Fail:    dif[0]=N_FAI[0]; dif[1]=N_FAI[1]; dif[2]=N_FAI[2]; break;
        }
        dif[3] = 1.0f;

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  AMB);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  dif);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, SPC);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 24.0f); // орташа жылтырау
        static constexpr float EM0[4] = {0,0,0,1};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, EM0);
    }
}
