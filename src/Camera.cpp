#include "Camera.h"

void CameraController::update(GLFWwindow *window, const double delta_time, Camera &camera) const
{
    const auto right = glm::cross(camera.m_forward, camera.m_up);
    const auto distance = static_cast<float>(delta_time * m_move_speed);
    const auto angle = static_cast<float>(delta_time * m_turn_speed);
    const auto rotation = glm::quat(glm::vec3(0.0, angle, 0.0));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.m_eye += camera.m_forward * distance;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.m_eye -= camera.m_forward * distance;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.m_eye += right * distance;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.m_eye -= right * distance;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.m_forward = glm::inverse(rotation) * camera.m_forward;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.m_forward = rotation * camera.m_forward;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.m_eye += camera.m_up * distance;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera.m_eye -= camera.m_up * distance;
    }
}
