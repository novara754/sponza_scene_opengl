#version 330 core

in vec2 o_tex_coords;

uniform sampler2D screen_texture;

out vec4 frag_color;

vec3 grayscale(vec3 color) {
    float avg = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    return vec3(avg);
}

void main() {
    vec3 color = texture(screen_texture, o_tex_coords).rgb;
    //    color = grayscale(color);
    frag_color = vec4(color, 1.0);
}
