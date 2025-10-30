#version 330 core
layout (location=0) in vec3 a;
layout (location=1) in vec3 b;
layout (location=2) in float w;

uniform mat4 uView;
uniform mat4 uProj;

out float vW;

void main(){
    vW = w;
    // GL_LINES әр екі вершина жеке: a, b -> бөлек вершина ретінде беріледі
    vec3 pos = (gl_VertexID % 2 == 0) ? a : b;
    gl_Position = uProj * uView * vec4(pos, 1.0);
}
