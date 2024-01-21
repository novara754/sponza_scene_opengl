#include "App.h"

#include <stdexcept>

#include <GLFW/glfw3.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.inl>
#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

glm::vec3 assimp_to_glm(aiVector3D vec)
{
    return {vec.x, vec.y, vec.z};
}

App::App(GLFWwindow *window) : m_window(window)
{
    const auto *scene = m_assimp_importer.ReadFile(
        "./assets/sponza.gltf",
        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs
    );
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error("failed to open scene");
    }

    for (auto i = 0; i < scene->mNumMaterials; ++i)
    {
        const auto *material = scene->mMaterials[i];
        aiString diffuse_name;

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse_name) != aiReturn_SUCCESS)
            {
                throw std::runtime_error(fmt::format("failed to get texture for material #{}", i));
            }
        } else
        {
            diffuse_name = "white.png";
        }

        const auto diffuse_path = std::string("./assets/") + diffuse_name.C_Str();

        const auto diffuse = Texture::from_file_2d(diffuse_path);

        m_materials.push_back(std::make_shared<Material>(diffuse));
    }

    std::vector<Mesh> meshes;
    meshes.reserve(scene->mRootNode->mNumMeshes);
    for (auto i = 0; i < scene->mRootNode->mNumMeshes; ++i)
    {
        const auto mesh_idx = scene->mRootNode->mMeshes[i];
        const auto *mesh = scene->mMeshes[mesh_idx];

        std::vector<Mesh::Vertex> vertices;
        vertices.reserve(mesh->mNumVertices);
        for (auto j = 0; j < mesh->mNumVertices; ++j)
        {
            Mesh::Vertex vertex{
                .position = assimp_to_glm(mesh->mVertices[j]),
                .normal = assimp_to_glm(mesh->mNormals[j]),
                .tex_coords = {0.0f, 0.0f},
            };
            if (const auto tex_coords = mesh->mTextureCoords[0])
            {
                vertex.tex_coords.x = tex_coords[j].x;
                vertex.tex_coords.y = tex_coords[j].y;
            }
            vertices.emplace_back(vertex);
        }

        std::vector<std::uint32_t> indices;
        for (auto j = 0; j < mesh->mNumFaces; ++j)
        {
            const auto face = mesh->mFaces[j];
            for (auto k = 0; k < face.mNumIndices; ++k)
            {
                indices.emplace_back(face.mIndices[k]);
            }
        }

        const auto material = m_materials[mesh->mMaterialIndex];

        meshes.emplace_back(vertices, indices, material);
    }
    m_models.emplace_back(meshes, Transform({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}));

    m_depth_program.attach_shader(GL_VERTEX_SHADER, "./shaders/depth.vert.glsl");
    m_depth_program.attach_shader(GL_FRAGMENT_SHADER, "./shaders/depth.frag.glsl");
    m_depth_program.link();

    m_shadow_map_framebuffer.set_depth_attachment(m_shadow_map_depth_attachment);
    m_shadow_map_framebuffer.set_draw_buffer(GL_NONE);
    m_shadow_map_framebuffer.set_read_buffer(GL_NONE);

    m_post_processing_framebuffer.set_color_attachment(m_post_processing_color_attachment);
    m_post_processing_framebuffer.set_depth_attachment(m_post_processing_depth_attachment);

    m_phong_program.attach_shader(GL_VERTEX_SHADER, "./shaders/phong.vert.glsl");
    m_phong_program.attach_shader(GL_FRAGMENT_SHADER, "./shaders/phong.frag.glsl");
    m_phong_program.link();

    m_post_processing_program.attach_shader(GL_VERTEX_SHADER, "./shaders/postprocessing.vert.glsl");
    m_post_processing_program.attach_shader(
        GL_FRAGMENT_SHADER,
        "./shaders/postprocessing.frag.glsl"
    );
    m_post_processing_program.link();
}

int App::run()
{
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    glViewport(0, 0, width, height);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageControl(
        GL_DONT_CARE,
        GL_DONT_CARE,
        GL_DEBUG_SEVERITY_NOTIFICATION,
        0,
        nullptr,
        GL_FALSE
    );
    glDebugMessageCallback(debug_message_callback, nullptr);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init();

    auto last_frame_time = glfwGetTime();
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);

        const auto now = glfwGetTime();
        const auto delta_time = now - last_frame_time;
        m_camera_controller.update(m_window, delta_time, m_camera);

        render(delta_time);

        last_frame_time = now;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return EXIT_SUCCESS;
}

void App::render(const double delta_time)
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Shadow Map Render Pass");
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    m_shadow_map_framebuffer.bind();
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        glClearColor(0.2, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_depth_program.use();
        m_depth_program.set_uniform("light_space", m_sun.get_light_space_matrix());

        for (const auto &model : m_models)
        {
            model.draw(m_depth_program);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPopDebugGroup();

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Scene Render Pass");
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    m_post_processing_framebuffer.bind();
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_phong_program.use();
        m_phong_program.set_uniform("view", m_camera.get_view_matrix());
        m_phong_program.set_uniform("projection", m_camera.get_projection_matrix());
        m_phong_program.set_uniform("camera_position", m_camera.m_eye);

        m_phong_program.set_uniform("light.position", m_light.m_position);
        m_phong_program.set_uniform("light.ambient", m_light.m_ambient);
        m_phong_program.set_uniform("light.diffuse", m_light.m_diffuse);
        m_phong_program.set_uniform("light.specular", m_light.m_specular);
        m_phong_program.set_uniform("light.constant", m_light.m_constant_attenuation);
        m_phong_program.set_uniform("light.linear", m_light.m_linear_attenuation);
        m_phong_program.set_uniform("light.quadratic", m_light.m_quadratic_attenuation);

        m_phong_program.set_uniform("material.diffuse_map", 0);
        m_phong_program.set_uniform("material.shininess", 64.0f);

        m_phong_program.set_uniform("sun.direction", m_sun.m_direction);
        m_phong_program.set_uniform("sun.ambient", m_sun.m_ambient);
        m_phong_program.set_uniform("sun.diffuse", m_sun.m_diffuse);
        m_phong_program.set_uniform("sun.specular", m_sun.m_specular);
        m_phong_program.set_uniform("sun.shadow_map", 2);
        m_phong_program.set_uniform("light_space", m_sun.get_light_space_matrix());

        m_shadow_map_depth_attachment.bind(GL_TEXTURE2);

        for (const auto &model : m_models)
        {
            model.draw(m_phong_program);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPopDebugGroup();

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Post-Processing Render Pass");
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        m_post_processing_program.use();
        m_post_processing_color_attachment.bind(GL_TEXTURE0);
        m_post_processing_plane.draw();
    }
    glPopDebugGroup();

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "ImGui Render Pass");
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        draw_ui(delta_time);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    glPopDebugGroup();
}

void App::draw_ui(const double delta_time)
{
    ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    {
        ImGui::Text("FPS: %.1f", 1.0 / delta_time);

        ImGui::SeparatorText("Camera");
        ImGui::InputFloat3(
            "Position",
            glm::value_ptr(m_camera.m_eye),
            "%.3f",
            ImGuiInputTextFlags_ReadOnly
        );
    }
    ImGui::End();

    ImGui::Begin("Light", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    {
        ImGui::SeparatorText("Transform");
        ImGui::SliderFloat3("Position", glm::value_ptr(m_light.m_position), -10.0f, 10.0f);

        ImGui::SeparatorText("Color w/ Intensity");
        ImGui::ColorEdit3("Ambient", glm::value_ptr(m_light.m_ambient));
        ImGui::ColorEdit3("Diffuse", glm::value_ptr(m_light.m_diffuse));
        ImGui::ColorEdit3("Specular", glm::value_ptr(m_light.m_specular));

        ImGui::SeparatorText("Attenuation");
        ImGui::SliderFloat("Constant", &m_light.m_constant_attenuation, 0.0f, 2.0f);
        ImGui::SliderFloat("Linear", &m_light.m_linear_attenuation, 0.0f, 2.0f);
        ImGui::SliderFloat("Quadratic", &m_light.m_quadratic_attenuation, 0.0f, 2.0f);
    }
    ImGui::End();

    ImGui::Begin("Sun", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    {
        ImGui::SeparatorText("Transform");
        ImGui::SliderFloat3("Position", glm::value_ptr(m_sun.m_position), -3'000.0f, 3'000.0f);
        ImGui::SliderFloat3("Direction", glm::value_ptr(m_sun.m_direction), -3'000.0f, 3'000.0f);

        ImGui::SeparatorText("Color w/ Intensity");
        ImGui::ColorEdit3("Ambient", glm::value_ptr(m_sun.m_ambient));
        ImGui::ColorEdit3("Diffuse", glm::value_ptr(m_sun.m_diffuse));
        ImGui::ColorEdit3("Specular", glm::value_ptr(m_sun.m_specular));

        ImGui::SeparatorText("Projection");
        ImGui::SliderFloat("Left/Right", &m_sun.m_left_right, 0.0f, 10'000.0f);
        ImGui::SliderFloat("Top/Bottom", &m_sun.m_top_bottom, 0.0f, 10'000.0f);
        ImGui::SliderFloat("Z Far", &m_sun.m_z_far, 0.0f, 100'000.0f);

        ImGui::SeparatorText("Shadow Map");
        ImGui::Image(
            reinterpret_cast<void *>(m_shadow_map_depth_attachment.get_handle()),
            ImVec2(256, 256)
        );
    }
    ImGui::End();
}

void App::framebuffer_size_callback(GLFWwindow *window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

void GLAPIENTRY App::debug_message_callback(
    const GLenum source, const GLenum in_msg_type, const GLuint id, const GLenum in_severity,
    const GLsizei length, const GLchar *message, const void *user_param
)
{
    const auto msg_type = [in_msg_type]() {
        switch (in_msg_type)
        {
            case GL_DEBUG_TYPE_ERROR:
                return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                return "DEPREC";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                return "UNDEF";
            case GL_DEBUG_TYPE_PORTABILITY:
                return "PORTAB";
            case GL_DEBUG_TYPE_PERFORMANCE:
                return "PERFOR";
            case GL_DEBUG_TYPE_MARKER:
                return "MARKER";
            case GL_DEBUG_TYPE_PUSH_GROUP:
                return "PUSH_G";
            case GL_DEBUG_TYPE_POP_GROUP:
                return "POP_G";
            case GL_DEBUG_TYPE_OTHER:
                return "OTHER";
            default:
                throw std::runtime_error("unreachable");
        }
    }();

    const auto severity = [in_severity]() {
        switch (in_severity)
        {
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                return "NOTI";
            case GL_DEBUG_SEVERITY_LOW:
                return "LOW";
            case GL_DEBUG_SEVERITY_MEDIUM:
                return "MED";
            case GL_DEBUG_SEVERITY_HIGH:
                return "HIGH";
            default:
                throw std::runtime_error("unreachable");
        }
    }();

    std::string msg(message, length);

    if (msg.ends_with(" is being recompiled based on GL state."))
    {
        return;
    }

    spdlog::info("[{}] [{}] {}", msg_type, severity, msg);
}
