#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../core/node.cpp"
#include "../utils/shader_loader.cpp"

struct NodeRenderer {
    GLuint vao=0, vbo=0;
    Shader shader;

    void init(const std::string& shaderDir) {
        shader = Shader(shaderDir + "/node.vert", shaderDir + "/node.frag");
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
    }

    void draw(const std::vector<Node>& nodes,
              const glm::mat4& view, const glm::mat4& proj, float t)
    {
        struct GPUNode { glm::vec3 pos; float size; glm::vec3 color; float weight; };
        std::vector<GPUNode> data;
        data.reserve(nodes.size());
        for (auto& n: nodes) data.push_back({n.pos, n.size, n.color, n.weight});

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(GPUNode), data.data(), GL_DYNAMIC_DRAW);

        // layout (location = 0) vec3 pos; (location=1) float size; (location=2) vec3 color; (location=3) float weight;
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUNode), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GPUNode), (void*)offsetof(GPUNode,size));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GPUNode), (void*)offsetof(GPUNode,color));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUNode), (void*)offsetof(GPUNode,weight));

        shader.use();
        shader.setMat4("uView", &view[0][0]);
        shader.setMat4("uProj", &proj[0][0]);
        shader.setFloat("uTime", t);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, (GLsizei)nodes.size());

        glBindVertexArray(0);
    }
};
