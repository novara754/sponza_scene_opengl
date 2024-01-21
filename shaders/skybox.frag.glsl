#version 330 core

in vec3 o_tex_coords;

uniform samplerCube cubemap;

out vec4 frag_color;

void main()
{
    frag_color = texture(cubemap, o_tex_coords);
}
