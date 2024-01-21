#ifndef APP_H
#define APP_H

#include <memory>

#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

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
    static constexpr int SHADOW_MAP_SIZE = 4096;
    static constexpr std::uint32_t WINDOW_WIDTH = 1280;
    static constexpr std::uint32_t WINDOW_HEIGHT = 720;

  private:
    Assimp::Importer m_assimp_importer;

    GLFWwindow *m_window;

    Camera m_camera{
        .m_eye = {1170.0f, 145.0f, -40.0f},
        .m_forward = {-1.0f, 0.0f, 0.0f},
        .m_up = {0.0f, 1.0f, 0.0f},
        .m_aspect = 1280.0f / 720.0f,
        .m_fov_y = 45.0f,
        .m_z_near = 0.1f,
        .m_z_far = 10'000.0f,
    };
    CameraController m_camera_controller;

    DirectionalLight m_sun{
        .m_position = {20.0f, 3000.0f, -1.0f},
        .m_direction = {-20.0f, -3000.0f, 1.0f},
        .m_color = {1.0f, 1.0f, 1.0f},
        .m_diffuse = 5.0f,
        .m_ambient = 0.025f,
        .m_specular = 0.8f,
        .m_left_right = 3000.0f,
        .m_top_bottom = 3000.0f,
        .m_z_near = 0.1f,
        .m_z_far = 10000.0f,
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
    float m_gamma{2.2};

    ShaderProgram m_phong_program;
    std::vector<Model> m_models;
    std::vector<std::shared_ptr<Material>> m_materials;

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
