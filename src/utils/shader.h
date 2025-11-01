#pragma once
#include <string>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;
    Shader(const char* vPath, const char* fPath) {
        std::string vCode, fCode;
        std::ifstream vFile(vPath), fFile(fPath);
        std::stringstream vs, fs;
        vs << vFile.rdbuf(); fs << fFile.rdbuf();
        vCode = vs.str(); fCode = fs.str();

        unsigned int vShader = compile(GL_VERTEX_SHADER, vCode.c_str());
        unsigned int fShader = compile(GL_FRAGMENT_SHADER, fCode.c_str());
        ID = glCreateProgram();
        glAttachShader(ID, vShader);
        glAttachShader(ID, fShader);
        glLinkProgram(ID);
        glDeleteShader(vShader);
        glDeleteShader(fShader);
    }

    void use() const { glUseProgram(ID); }

private:
    static unsigned int compile(GLenum type, const char* src) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        return shader;
    }
};
