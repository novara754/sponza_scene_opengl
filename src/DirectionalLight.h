#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 m_position;
    glm::vec3 m_rotation;

    glm::vec3 m_color;
    glm::vec3 m_ambient;
    float m_diffuse;
    float m_specular;

    float m_left_right;
    float m_top_bottom;
    float m_z_near;
    float m_z_far;

    [[nodiscard]] glm::mat4 get_light_space_matrix() const
    {
        // ReSharper disable once CppRedundantQualifier
        const auto view =
            glm::lookAtRH(m_position, m_position + get_direction(), glm::vec3(0.0f, 1.0f, 0.0f));
        const auto projection = glm::ortho(-m_left_right, m_left_right, -m_top_bottom, m_top_bottom, m_z_near, m_z_far);
        return projection * view;
    }

    [[nodiscard]] glm::vec3 get_direction() const
    {
        return glm::quat(m_rotation / 180.0f * 3.141592f) * glm::vec3(0.0, 0.0, 1.0);
    }
};

#endif // DIRECTIONALLIGHT_H
