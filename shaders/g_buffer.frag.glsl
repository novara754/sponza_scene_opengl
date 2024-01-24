#version 330 core

struct Material {
    sampler2D diffuse_map;
    sampler2D normal_map;
};

in vec4 o_frag_position;
in vec2 o_tex_coords;
in mat3 o_tbn;

uniform vec3 camera_position;

uniform Material material;

layout (location = 0) out vec4 frag_albedo;
layout (location = 1) out vec4 frag_position;
layout (location = 2) out vec3 frag_normal;

vec3 get_normal() {
    vec3 normal = texture(material.normal_map, o_tex_coords).rgb;
    normal = normal * 2.0 - 1.0;
    return normalize(o_tbn * normal);
}

void main() {
    frag_albedo = texture(material.diffuse_map, o_tex_coords);
    frag_position = o_frag_position;
    frag_normal = get_normal();
}
