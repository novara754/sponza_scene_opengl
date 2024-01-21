#version 330 core

layout (location = 0) in vec3 a_position;

uniform mat4 projection;
uniform mat4 view;

out vec3 o_tex_coords;

void main()
{
    o_tex_coords = a_position;
    vec4 position = projection * view * vec4(a_position, 1.0);
    gl_Position = position.xyww;
}
