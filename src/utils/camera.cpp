#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct OrbitCamera {
    float distance = 3.0f;
    float yaw = 0.8f;   // radians
    float pitch = 0.35f;

    glm::mat4 view() const {
        glm::vec3 target(0.0f);
        glm::vec3 pos = target + glm::vec3(
            distance * cosf(pitch) * sinf(yaw),
            distance * sinf(pitch),
            distance * cosf(pitch) * cosf(yaw)
        );
        return glm::lookAt(pos, target, glm::vec3(0,1,0));
    }
    void addYaw(float dy)   { yaw += dy; }
    void addPitch(float dp) { pitch = glm::clamp(pitch + dp, -1.2f, 1.2f); }
    void addZoom(float dz)  { distance = glm::clamp(distance + dz, 1.5f, 8.0f); }
};
