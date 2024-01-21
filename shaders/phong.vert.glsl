#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 light_space;

out vec3 o_frag_position;
out vec4 o_light_space_frag_position;
out vec3 o_normal;
out vec2 o_tex_coords;

void main() {
    vec4 frag_position = model * vec4(a_position, 1.0);
    o_frag_position = frag_position.xyz;
    o_light_space_frag_position = light_space * frag_position;
    o_normal = transpose(inverse(mat3(model))) * a_normal;
    o_tex_coords = a_tex_coords;

    gl_Position = projection * view * frag_position;
}
