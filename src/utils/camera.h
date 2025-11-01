#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Ray3D { glm::vec3 o; glm::vec3 d; }; // origin, dir (normalized)

class Camera3D {
public:
    glm::vec3 target{0.0f, 0.0f, 0.0f};
    float distance = 3.5f;
    float yaw = 0.60f;
    float pitch = -0.25f;

    float fov_deg = 50.0f;
    float znear = 0.05f;
    float zfar  = 100.0f;

    void pan(float dx, float dy) {
        glm::vec3 f = getForward();
        glm::vec3 r = glm::normalize(glm::cross(f, {0,1,0}));
        glm::vec3 u = glm::normalize(glm::cross(r, f));
        target += (-dx * r) + (dy * u);
    }
    void orbit(float dYaw, float dPitch) {
        yaw   += dYaw;
        pitch += dPitch;
        const float lim = 1.2f;
        if (pitch >  lim) pitch =  lim;
        if (pitch < -lim) pitch = -lim;
    }
    void dolly(float factor) {
        distance *= factor;
        if (distance < 1.0f)  distance = 1.0f;
        if (distance > 30.0f) distance = 30.0f;
    }

    // Matrices
    glm::mat4 proj(int w, int h) const {
        float aspect = (h > 0) ? (float)w / (float)h : 1.0f;
        return glm::perspective(glm::radians(fov_deg), aspect, znear, zfar);
    }
    glm::mat4 view() const {
        return glm::lookAt(getEye(), target, glm::vec3(0,1,0));
    }
    void apply(int w, int h) const {
        glm::mat4 P = proj(w,h), V = view();
        glMatrixMode(GL_PROJECTION); glLoadIdentity(); glMultMatrixf(glm::value_ptr(P));
        glMatrixMode(GL_MODELVIEW);  glLoadIdentity(); glMultMatrixf(glm::value_ptr(V));
    }

    glm::vec3 getEye() const {
        return target - getForward() * distance;
    }
    glm::vec3 getForward() const {
        float cp = cosf(pitch), sp = sinf(pitch);
        float cy = cosf(yaw),   sy = sinf(yaw);
        return glm::normalize(glm::vec3(cp*cy, sp, cp*sy));
    }

    // Screen → Ray (GL-style coords: mouse (x,y) pixels from top-left)
    Ray3D rayFromScreen(double x, double y, int w, int h) const {
        float ndc_x =  2.0f * (float)x / (float)w - 1.0f;
        float ndc_y =  1.0f - 2.0f * (float)y / (float)h; // invert Y
        glm::mat4 P = proj(w,h), V = view(), invPV = glm::inverse(P*V);

        glm::vec4 np = invPV * glm::vec4(ndc_x, ndc_y, -1.0f, 1.0f);
        glm::vec4 fp = invPV * glm::vec4(ndc_x, ndc_y,  1.0f, 1.0f);
        np /= np.w; fp /= fp.w;

        glm::vec3 o = glm::vec3(np);
        glm::vec3 d = glm::normalize(glm::vec3(fp - np));
        return {o, d};
    }
};
