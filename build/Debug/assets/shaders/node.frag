#version 330 core
in vec3 vColor;
in float vSize;
out vec4 FragColor;

void main(){
    // Point sprite ішінде шеңбер салу
    vec2 uv = gl_PointCoord*2.0 - 1.0;
    float r = length(uv);
    if (r>1.0) discard;

    float edge = smoothstep(1.0, 0.9, r);
    vec3 col = mix(vec3(1.0), vColor, edge);

    // аздап жарқыл
    float glow = smoothstep(0.2, 0.0, r)*0.25;
    FragColor = vec4(col + glow, 1.0);
}
