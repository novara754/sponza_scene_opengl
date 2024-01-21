#version 330 core

in vec2 o_tex_coords;

uniform sampler2D image;
uniform bool horizontal;

float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 frag_color;

void main() {
    vec2 tex_offset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, o_tex_coords).rgb * weights[0];

    if (horizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(image, o_tex_coords + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
            result += texture(image, o_tex_coords - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(image, o_tex_coords + vec2(0.0, tex_offset.y * i)).rgb * weights[i];
            result += texture(image, o_tex_coords - vec2(0.0, tex_offset.y * i)).rgb * weights[i];
        }
    }

    frag_color = vec4(result, 1.0);
}
