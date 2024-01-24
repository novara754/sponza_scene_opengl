#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 2) in vec2 a_tex_coords;

out vec2 o_tex_coords;

void main() {
    o_tex_coords = a_tex_coords;
    gl_Position = vec4(a_position.x, a_position.y, 0.0, 1.0);
}
