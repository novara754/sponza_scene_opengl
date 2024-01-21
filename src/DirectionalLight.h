#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 m_position;
    glm::vec3 m_direction;

    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    [[nodiscard]] glm::mat4 get_light_space_matrix() const
    {
        // ReSharper disable once CppRedundantQualifier
        const auto view =
            glm::lookAtRH(m_position, m_position + m_direction, glm::vec3(0.0f, 1.0f, 0.0f));
        // glm::lookAt(
        //     glm::vec3(-2.0f, 4.0f, -1.0f),
        //     glm::vec3(0.0f, 0.0f, 0.0f),
        //     glm::vec3(0.0f, 1.0f, 0.0f)
        // );
        const auto projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
        return projection * view;
    }
};

#endif // DIRECTIONALLIGHT_H
