#version 330 core

out vec4 frag_color;

void main() {
    gl_FragDepth = gl_FragCoord.z;
    frag_color = vec4(1.0);
}
