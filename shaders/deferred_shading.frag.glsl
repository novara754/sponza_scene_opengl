#version 330 core

struct DirectionalLight {
    vec3 direction;

    vec3 color;
    vec3 ambient;
    float diffuse;
    float specular;

    mat4 transform;
    sampler2D shadow_map;
};

in vec2 o_tex_coords;

uniform DirectionalLight sun;

uniform vec3 camera_position;

uniform sampler2D albedo_map;
uniform sampler2D positions_map;
uniform sampler2D normals_map;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

float shadow(DirectionalLight light, vec4 position, vec3 normal) {
    vec4 light_space_position = light.transform * position;
    vec3 proj_coords = light_space_position.xyz / light_space_position.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    if (proj_coords.z > 1.0) {
        return 0.0;
    }

    float current_depth = proj_coords.z;
    vec3 sun_dir = -light.direction;
    float bias = max(0.05 * (1.0 - dot(normal, sun_dir)), 0.005);

    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(light.shadow_map, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcf_depth = texture(light.shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += (current_depth - bias > pcf_depth) ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;
}

vec3 calc_directional_light(DirectionalLight light) {
    vec3 diffuse_reflection = texture(albedo_map, o_tex_coords).rgb;
    vec3 specular_reflection = vec3(0.0);
    vec3 normal = texture(normals_map, o_tex_coords).xyz;
    vec4 position = texture(positions_map, o_tex_coords);

    vec3 ambient = diffuse_reflection * light.ambient;

    vec3 light_dir = normalize(-light.direction);
    vec3 diffuse = max(dot(normal, light_dir), 0.0) * diffuse_reflection * light.diffuse * light.color;

    vec3 camera_dir = normalize(camera_position - position.xyz);
    vec3 half_dir = normalize(light_dir + camera_dir);
    float specular_strength = pow(max(dot(normal, half_dir), 0.0), 64.0);
    vec3 specular = specular_strength * specular_reflection * light.specular * light.color;

    return ambient + (1.0 - shadow(light, position, normal)) * (diffuse + specular);
}

void main()
{
    vec3 color = calc_directional_light(sun);
    frag_color = vec4(color, 1.0);

    float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        bright_color = frag_color;
    } else {
        bright_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
