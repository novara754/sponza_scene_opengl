#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

struct Camera
{
    glm::vec3 m_eye;
    float m_yaw;
    float m_pitch;
    glm::vec3 m_up;
    float m_aspect;
    float m_fov_y;
    float m_z_near;
    float m_z_far;

    [[nodiscard]] glm::vec3 get_forward() const
    {
        return glm::normalize(glm::vec3(
            std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch)),
            std::sin(glm::radians(m_pitch)),
            std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch))
        ));;
    }

    [[nodiscard]] glm::mat4 get_view_matrix() const
    {
        const auto forward = get_forward();
        // ReSharper disable once CppRedundantQualifier
        return glm::lookAtRH(m_eye, m_eye + forward, m_up);
    }

    [[nodiscard]] glm::mat4 get_projection_matrix() const
    {
        return glm::perspective(m_fov_y, m_aspect, m_z_near, m_z_far);
    }
};

class CameraController
{
    float m_move_speed{1000.0f};
    float m_look_sensitivity{0.1f};

    glm::vec2 m_last_mouse_pos;
    glm::vec2 m_mouse_delta{0.0f, 0.0f};

  public:
    CameraController(int window_width, int window_height);

    void update(GLFWwindow *window, double delta_time, Camera &camera);

    void update_mouse(float x, float y);
};

#endif // CAMERA_H
