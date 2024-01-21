#ifndef MODEL_H
#define MODEL_H

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Mesh.h"
#include "ShaderProgram.h"

struct Transform
{
    glm::vec3 m_position{0.0f, 0.0f, 0.0f};
    glm::vec3 m_rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 m_scale{1.0f, 1.0f, 1.0f};

    [[nodiscard]] glm::mat4 get_model_matrix() const
    {
        const auto scale = glm::scale(glm::mat4(1.0f), m_scale);
        const auto rotate = glm::mat4(glm::quat(m_rotation / 180.0f * 3.141592f)) * scale;
        const auto translate = glm::translate(rotate, m_position);
        return translate;
    }
};

struct Model
{
    std::vector<Mesh> m_meshes;
    Transform m_transform;

    void draw(ShaderProgram &program) const
    {
        program.set_uniform("model", m_transform.get_model_matrix());
        for (const auto &mesh : m_meshes)
        {
            mesh.draw();
        }
    }
};

#endif // MODEL_H
