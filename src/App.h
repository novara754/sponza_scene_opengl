#ifndef APP_H
#define APP_H

#include <GLFW/glfw3.h>

#include "Camera.h"
#include "DirectionalLight.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "Model.h"
#include "PointLight.h"
#include "ShaderProgram.h"
#include "Texture.h"

class App
{
  public:
    static constexpr int SHADOW_MAP_SIZE = 1024;
    static constexpr std::uint32_t WINDOW_WIDTH = 1280;
    static constexpr std::uint32_t WINDOW_HEIGHT = 720;

  private:
    GLFWwindow *m_window;

    Camera m_camera{
        .m_eye = {0.0f, 0.0f, -3.0f},
        .m_forward = {0.0f, 0.0f, 1.0f},
        .m_up = {0.0f, 1.0f, 0.0f},
        .m_aspect = 1280.0f / 720.0f,
        .m_fov_y = 45.0f,
        .m_z_near = 0.1f,
        .m_z_far = 100.0f,
    };
    CameraController m_camera_controller;

    DirectionalLight m_sun{
        .m_position = {-2.0f, 4.0f, -1.0f},
        .m_direction = {2.0f, -4.0f, 1.0f},
        .m_ambient = {0.1f, 0.1f, 0.1f},
        .m_diffuse = {0.8f, 0.8f, 0.8f},
        .m_specular = {0.8f, 0.8f, 0.8f},
    };

    ShaderProgram m_depth_program;
    Texture m_shadow_map_depth_attachment{
        Texture::depth_attachment(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE)
    };
    Framebuffer m_shadow_map_framebuffer;

    Texture m_post_processing_color_attachment{Texture::color_attachment(1280, 720)};
    Texture m_post_processing_depth_attachment{Texture::depth_attachment(1280, 720)};
    Framebuffer m_post_processing_framebuffer;

    ShaderProgram m_post_processing_program;
    Mesh m_post_processing_plane{Mesh::plane()};

    ShaderProgram m_cube_program;
    Model m_cube{Mesh::cube(), Transform()};
    Model m_ground{
        Mesh::plane(), Transform({0.0f, 0.0f, -0.5f}, {-90.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 1.0f})
    };
    float m_shininess{64.0f};

    Texture m_container_diffuse{Texture::from_file_2d("./assets/container_diffuse.png")};
    Texture m_container_specular{Texture::from_file_2d("./assets/container_specular.png")};

    PointLight m_light{
        .m_position = {1.2f, 0.0f, -2.0f},
        .m_ambient = {0.1f, 0.1f, 0.1f},
        .m_diffuse = {0.4f, 0.4f, 0.4f},
        .m_specular = {0.5f, 0.5f, 0.5f},
        .m_constant_attenuation = 1.0f,
        .m_linear_attenuation = 0.09f,
        .m_quadratic_attenuation = 0.032f,
    };

  public:
    explicit App(GLFWwindow *window);
    int run();

  private:
    void render(const double delta_time);
    void draw_ui(const double delta_time);

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    static void GLAPIENTRY debug_message_callback(
        GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
        const GLchar *message, const void *userParam
    );
};

#endif // APP_H
