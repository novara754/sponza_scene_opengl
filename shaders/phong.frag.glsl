#version 330 core

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 direction;

    vec3 color;
    float ambient;
    float diffuse;
    float specular;

    sampler2D shadow_map;
};

struct Material {
    sampler2D diffuse_map;
    float shininess;
};

in vec3 o_frag_position;
in vec4 o_light_space_frag_position;
in vec3 o_normal;
in vec2 o_tex_coords;

uniform vec3 camera_position;

uniform DirectionalLight sun;
uniform PointLight light;
uniform Material material;

out vec4 frag_color;

float shadow(DirectionalLight light, vec3 normal) {
    vec3 proj_coords = o_light_space_frag_position.xyz / o_light_space_frag_position.w;
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
    vec3 diffuse_reflection = texture(material.diffuse_map, o_tex_coords).rgb;
    vec3 specular_reflection = vec3(0.0);
    vec3 normal = normalize(o_normal);

    vec3 ambient = diffuse_reflection * light.ambient * light.color;

    vec3 light_dir = normalize(-light.direction);
    vec3 diffuse = max(dot(normal, light_dir), 0.0) * diffuse_reflection * light.diffuse * light.color;

    vec3 camera_dir = normalize(camera_position - o_frag_position);
    vec3 half_dir = normalize(light_dir + camera_dir);
    float specular_strength = pow(max(dot(normal, half_dir), 0.0), material.shininess);
    vec3 specular = specular_strength * specular_reflection * light.specular * light.color;

    return ambient + (1.0 - shadow(light, normal)) * (diffuse + specular);
}

vec3 calc_point_light(PointLight light) {
    vec3 diffuse_reflection = texture(material.diffuse_map, o_tex_coords).rgb;
    vec3 specular_reflection = vec3(0.0);
    vec3 normal = normalize(o_normal);

    vec3 ambient = diffuse_reflection * light.ambient;

    vec3 light_dir = normalize(light.position - o_frag_position);
    vec3 diffuse = max(dot(light_dir, normal), 0.0) * diffuse_reflection * light.diffuse;

    vec3 camera_dir = normalize(camera_position - o_frag_position);
    vec3 half_dir = normalize(light_dir + camera_dir);
    float specular_strength = pow(max(dot(normal, half_dir), 0.0), material.shininess);
    vec3 specular = specular_strength * specular_reflection * light.specular;

    float light_distance = length(light.position - o_frag_position);
    float attenuation_inv = light.constant + light.linear * light_distance + light.quadratic * light_distance * light_distance;

    return (ambient + (diffuse + specular)) / attenuation_inv;
}

void main() {
    vec3 color = vec3(0.0);
    color += calc_directional_light(sun);
    color += calc_point_light(light);
    frag_color = vec4(color, 1.0);
}
