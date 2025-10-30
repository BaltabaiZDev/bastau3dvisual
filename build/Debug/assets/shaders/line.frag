#version 330 core
in float vW;
out vec4 FragColor;
void main(){
    float w = clamp(vW*0.4, 0.2, 1.0);
    FragColor = vec4(0.6, 0.7, 0.9, 0.6) * vec4(w,w,w,1.0);
}
