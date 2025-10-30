#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../core/graph.cpp"
#include "../utils/shader_loader.cpp"
#include "node_renderer.cpp"

struct GraphRenderer {
    Shader lineShader;
    GLuint vao=0, vbo=0;
    NodeRenderer nodes;

    void init(const std::string& shaderDir) {
        nodes.init(shaderDir);
        lineShader = Shader(shaderDir + "/line.vert", shaderDir + "/line.frag");
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
    }

    void draw(const Graph& g, const glm::mat4& view, const glm::mat4& proj, float t) {
        // 1) edges
        struct P2 { glm::vec3 a; glm::vec3 b; float w; };
        std::vector<P2> segs; segs.reserve(g.edges.size());
        for (auto& e: g.edges) {
            segs.push_back({ g.nodes[e.a].pos, g.nodes[e.b].pos, e.weight });
        }

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, segs.size()*sizeof(P2), segs.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0); // a
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(P2), (void*)0);
        glEnableVertexAttribArray(1); // b
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(P2), (void*)offsetof(P2,b));
        glEnableVertexAttribArray(2); // w
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(P2), (void*)offsetof(P2,w));

        lineShader.use();
        lineShader.setMat4("uView", &view[0][0]);
        lineShader.setMat4("uProj", &proj[0][0]);
        glDrawArrays(GL_LINES, 0, (GLsizei)(segs.size()*2));

        glBindVertexArray(0);

        // 2) nodes
        nodes.draw(g.nodes, view, proj, t);
    }
};
