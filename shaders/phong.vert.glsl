#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;
layout (location = 3) in vec3 a_tangent;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 light_space;

out vec3 o_frag_position;
out vec4 o_light_space_frag_position;
out mat3 o_tbn;
out vec2 o_tex_coords;

void main() {
    vec4 frag_position = model * vec4(a_position, 1.0);
    o_frag_position = frag_position.xyz;
    o_light_space_frag_position = light_space * frag_position;
    o_tex_coords = a_tex_coords;

    vec3 bitangent = cross(a_normal, a_tangent);
    vec3 t = normalize(vec3(model * vec4(a_tangent, 0.0)));
    vec3 b = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 n = normalize(vec3(model * vec4(a_normal, 0.0)));
    o_tbn = mat3(t, b, n);

    gl_Position = projection * view * frag_position;
}
