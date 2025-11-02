#include "renderer/graph_renderer.h"
#include <glad/glad.h>
#include <glm/gtc/constants.hpp>
#include "../ui/theme.h"

static constexpr float kSphereR = 0.07f;

void GraphRenderer::beginLighting() {
    glEnable(GL_LIGHTING);

    // Глобал ambient — көлеңке тым қара болмасын
    GLfloat GlobalAmb[4] = { 0.18f, 0.18f, 0.20f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, GlobalAmb);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    // Негізгі бағытталған жарық (key light)
    glEnable(GL_LIGHT0);
    GLfloat L0_pos[4] = {  0.6f,  1.0f,  0.8f, 0.0f }; // w=0 → directional
    GLfloat L0_dif[4] = {  0.95f, 0.95f, 0.95f, 1.0f };
    GLfloat L0_spc[4] = {  0.35f, 0.35f, 0.35f, 1.0f };
    GLfloat L0_amb[4] = {  0.00f, 0.00f, 0.00f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, L0_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  L0_dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, L0_spc);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  L0_amb);

    // Қарсы жақтан жұмсақ «fill» жарық — жартылай көлеңке эффекті
    glEnable(GL_LIGHT1);
    GLfloat L1_pos[4] = { -0.7f, -0.4f, -0.6f, 0.0f };
    GLfloat L1_dif[4] = {  0.45f, 0.45f, 0.50f, 1.0f }; // әлсіз, салқын рең
    GLfloat L1_spc[4] = {  0.05f, 0.05f, 0.08f, 1.0f };
    GLfloat L1_amb[4] = {  0.00f, 0.00f, 0.00f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, L1_pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  L1_dif);
    glLightfv(GL_LIGHT1, GL_SPECULAR, L1_spc);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  L1_amb);
}

void GraphRenderer::endLighting() {
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

void GraphRenderer::setColorByState(NodeState st) {
    Theme::setMaterialByState(st);
}

void GraphRenderer::drawBounds(float B) {
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glColor4f(1,1,1,0.15f);
    glBegin(GL_LINES);
    const float s = B;
    // 12 ребра куба
    auto V=[&](float x,float y,float z){ glVertex3f(x,y,z); };
    // low square
    V(-s,-s,-s); V(+s,-s,-s);
    V(+s,-s,-s); V(+s,-s,+s);
    V(+s,-s,+s); V(-s,-s,+s);
    V(-s,-s,+s); V(-s,-s,-s);
    // high square
    V(-s,+s,-s); V(+s,+s,-s);
    V(+s,+s,-s); V(+s,+s,+s);
    V(+s,+s,+s); V(-s,+s,+s);
    V(-s,+s,+s); V(-s,+s,-s);
    // verticals
    V(-s,-s,-s); V(-s,+s,-s);
    V(+s,-s,-s); V(+s,+s,-s);
    V(+s,-s,+s); V(+s,+s,+s);
    V(-s,-s,+s); V(-s,+s,+s);
    glEnd();
}

void GraphRenderer::drawSphere(float r, int stacks, int slices) {
    const float PI = glm::pi<float>(), TWO_PI = 2.0f*PI;
    for (int i = 0; i < stacks; ++i) {
        float v0 = (float)i / stacks;
        float v1 = (float)(i+1) / stacks;
        float t0 = v0 * PI, t1 = v1 * PI;

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float u = (float)j / slices;
            float phi = u * TWO_PI;

            float x0 = sinf(t0)*cosf(phi), y0 = cosf(t0), z0 = sinf(t0)*sinf(phi);
            float x1 = sinf(t1)*cosf(phi), y1 = cosf(t1), z1 = sinf(t1)*sinf(phi);

            glNormal3f(x1,y1,z1); glVertex3f(r*x1, r*y1, r*z1);
            glNormal3f(x0,y0,z0); glVertex3f(r*x0, r*y0, r*z0);
        }
        glEnd();
    }
}

void GraphRenderer::render(const Graph& graph, const Camera3D& cam, int w, int h,
                           int hoveredId, const RenderOptions& ro) {

    // World size estimate (same formula as in Graph::update)
    int n = graph.count();
    float worldR = std::max(1.2f, 0.28f * std::cbrt((float)std::max(1,n)));
    float B = worldR + 0.6f;

    glEnable(GL_DEPTH_TEST);
    cam.apply(w, h);

    // Axes (subtle)
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glColor3f(1,0,0); glVertex3f(-2,0,0); glVertex3f(2,0,0);
        glColor3f(0,1,0); glVertex3f(0,-2,0); glVertex3f(0,2,0);
        glColor3f(0,0,1); glVertex3f(0,0,-2); glVertex3f(0,0,2);
    glEnd();

    if (ro.showBounds) drawBounds(B);

    // Edges (егер бар болса)
    if (ro.showEdges) {
        glDisable(GL_LIGHTING);
        glLineWidth(1.5f);
        glColor4f(1,1,1,0.35f);
        glBegin(GL_LINES);
        for (const auto& e : graph.getEdges()) {

            const auto& A = graph.getNodes()[e.from];
            const auto& C = graph.getNodes()[e.to];
            glVertex3f(A.pos.x, A.pos.y, A.pos.z);
            glVertex3f(C.pos.x, C.pos.y, C.pos.z);
        }
        glEnd();
    }

    // Lighting on for spheres
    beginLighting();

    for (const auto& n0 : graph.getNodes()) {
        setColorByState(n0.state);
        glPushMatrix();
        glTranslatef(n0.pos.x, n0.pos.y, n0.pos.z);
        drawSphere(kSphereR, 16, 22);
        glPopMatrix();

        // Hover halo (қалауыңызша)
        if (ro.haloHover && n0.id == hoveredId) {
            glDisable(GL_LIGHTING);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4f(1.0f, 1.0f, 0.2f, 0.10f);
            glPushMatrix();
            glTranslatef(n0.pos.x, n0.pos.y, n0.pos.z);
            // slightly larger sphere as glow
            drawSphere(kSphereR*1.35f, 12, 18);
            glPopMatrix();
            glDisable(GL_BLEND);
            beginLighting(); // restore
        }
    }

    endLighting();
}
