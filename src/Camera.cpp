#include "Camera.h"

#include <algorithm>
#include <spdlog/spdlog.h>

CameraController::CameraController(int window_width, int window_height)
    : m_last_mouse_pos(static_cast<float>(window_width) / 2.0f,
      static_cast<float>(window_height) / 2.0f)
{
}

void CameraController::update(GLFWwindow *window, const double delta_time, Camera &camera)
{
    const auto forward = camera.get_forward();
    const auto right = glm::cross(forward, camera.m_up);
    const auto distance = static_cast<float>(delta_time * m_move_speed);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.m_eye += forward * distance;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.m_eye -= forward * distance;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.m_eye += right * distance;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.m_eye -= right * distance;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.m_eye += camera.m_up * distance;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera.m_eye -= camera.m_up * distance;
    }

    camera.m_yaw += m_mouse_delta.x * m_look_sensitivity;
    camera.m_pitch += m_mouse_delta.y * m_look_sensitivity;

    if (camera.m_yaw > 360.0f)
    {
        camera.m_yaw = 0.0f;
    } else if (camera.m_yaw < 0.0f)
    {
        camera.m_yaw = 360.0f;
    }
    camera.m_pitch = std::clamp(camera.m_pitch, -89.0f, 89.0f);

    m_mouse_delta = {0.0f, 0.0f};
}

void CameraController::update_mouse(const float x, const float y)
{
    m_mouse_delta.x = x - m_last_mouse_pos.x;
    m_mouse_delta.y = m_last_mouse_pos.y - y;
    m_last_mouse_pos = {x, y};
}
