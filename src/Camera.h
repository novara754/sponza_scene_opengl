#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

struct Camera
{
    glm::vec3 m_eye;
    glm::vec3 m_forward;
    glm::vec3 m_up;
    float m_aspect;
    float m_fov_y;
    float m_z_near;
    float m_z_far;

    [[nodiscard]] glm::mat4 get_view_matrix() const
    {
        // ReSharper disable once CppRedundantQualifier
        return glm::lookAtRH(m_eye, m_eye + m_forward, m_up);
    }

    [[nodiscard]] glm::mat4 get_projection_matrix() const
    {
        return glm::perspective(m_fov_y, m_aspect, m_z_near, m_z_far);
    }
};

class CameraController
{
    float m_move_speed{10.0f};
    float m_turn_speed{2.0f};

  public:
    void update(GLFWwindow *window, double delta_time, Camera &camera) const;
};

#endif // CAMERA_H
