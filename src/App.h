#ifndef APP_H
#define APP_H

#include <array>
#include <memory>

#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>

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
        .m_aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
        .m_fov_y = 45.0f,
        .m_z_near = 0.1f,
        .m_z_far = 10'000.0f,
    };
    CameraController m_camera_controller;

    DirectionalLight m_sun{
        .m_position = {20.0f, 3000.0f, -1.0f},
        .m_rotation = {90.0f, 0.0f, 0.0f},
        .m_color = {1.0f, 0.8f, 0.52f},
        .m_ambient = {0.18f, 0.20f, 0.21f},
        .m_diffuse = 5.0f,
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

    int m_bloom_amount{1};
    ShaderProgram m_bloom_program;
    std::array<Texture, 2> m_bloom_ping_pong_attachments{
        Texture::color_attachment(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA16F, GL_RGBA),
        Texture::color_attachment(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA16F, GL_RGBA),
    };
    std::array<Framebuffer, 2> m_bloom_ping_pong_framebuffers;

    ShaderProgram m_geometry_program;
    Texture m_g_buffer_albedo{Texture::color_attachment(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_RGB)
    };
    Texture m_g_buffer_positions{
        Texture::color_attachment(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA16F, GL_RGBA)
    };
    Texture m_g_buffer_normals{
        Texture::color_attachment(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB16F, GL_RGB)
    };
    Texture m_g_buffer_depth{Texture::depth_attachment(WINDOW_WIDTH, WINDOW_HEIGHT)};
    Framebuffer m_geometry_buffer;

    ShaderProgram m_deferred_shading_program;

    Texture m_post_processing_color_attachment{
        Texture::color_attachment(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA16F, GL_RGBA)
    };
    Texture m_post_processing_color_attachment_bright{
        Texture::color_attachment(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA16F, GL_RGBA)
    };
    Framebuffer m_post_processing_framebuffer;

    ShaderProgram m_post_processing_program;
    Mesh m_post_processing_plane{Mesh::plane()};
    float m_gamma{2.2f};
    float m_exposure{1.0f};

    ShaderProgram m_skybox_program;
    std::shared_ptr<Texture> m_skybox_texture{Texture::from_file_cubemap(std::array<std::string, 6>{
        "./assets/skybox/px.png",
        "./assets/skybox/nx.png",
        "./assets/skybox/py.png",
        "./assets/skybox/ny.png",
        "./assets/skybox/pz.png",
        "./assets/skybox/nz.png",
    })};
    Mesh m_skybox_mesh{Mesh::skybox()};

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
