#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in float aSize;
layout (location=2) in vec3 aColor;
layout (location=3) in float aWeight;

uniform mat4 uView;
uniform mat4 uProj;
uniform float uTime;

out vec3 vColor;
out float vSize;

void main(){
    vColor = aColor;
    vSize = aSize + 0.005 * sin(uTime*4.0 + aWeight*2.0);
    gl_Position = uProj * uView * vec4(aPos, 1.0);
    gl_PointSize = (vSize * 100.0) / gl_Position.w; // perspective scale
}
