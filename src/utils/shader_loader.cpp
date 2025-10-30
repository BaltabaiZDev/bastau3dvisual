#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

inline std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) throw std::runtime_error("Cannot open: " + path);
    std::stringstream ss; ss << f.rdbuf(); return ss.str();
}

struct Shader {
    GLuint id = 0;

    Shader() = default;
    Shader(const std::string& vertPath, const std::string& fragPath) {
        compile(vertPath, fragPath);
    }

    void compile(const std::string& vertPath, const std::string& fragPath) {
        std::string vsrc = readFile(vertPath);
        std::string fsrc = readFile(fragPath);
        const char* v = vsrc.c_str();
        const char* f = fsrc.c_str();

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &v, nullptr); glCompileShader(vs);
        check(vs, "VERTEX");

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &f, nullptr); glCompileShader(fs);
        check(fs, "FRAGMENT");

        id = glCreateProgram();
        glAttachShader(id, vs); glAttachShader(id, fs);
        glLinkProgram(id); checkProgram();

        glDeleteShader(vs); glDeleteShader(fs);
    }

    void use() const { glUseProgram(id); }
    void setMat4(const char* name, const float* ptr) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, ptr);
    }
    void setVec3(const char* name, const float* ptr) const {
        glUniform3fv(glGetUniformLocation(id, name), 1, ptr);
    }
    void setFloat(const char* name, float v) const {
        glUniform1f(glGetUniformLocation(id, name), v);
    }
private:
    void check(GLuint shader, const std::string& type) {
        GLint ok; glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char buf[1024]; glGetShaderInfoLog(shader, 1024, nullptr, buf);
            throw std::runtime_error(type + " compile error:\n" + std::string(buf));
        }
    }
    void checkProgram() {
        GLint ok; glGetProgramiv(id, GL_LINK_STATUS, &ok);
        if (!ok) {
            char buf[1024]; glGetProgramInfoLog(id, 1024, nullptr, buf);
            throw std::runtime_error(std::string("Program link error:\n") + buf);
        }
    }
};
