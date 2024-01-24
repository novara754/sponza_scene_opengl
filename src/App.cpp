#include "App.h"

#include <array>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
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
        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace
    );
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error("failed to open scene");
    }

    for (auto i = 0; i < scene->mNumMaterials; ++i)
    {
        const auto *material = scene->mMaterials[i];
        aiString diffuse_name, normal_name;

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse_name) != aiReturn_SUCCESS)
            {
                throw std::runtime_error(
                    fmt::format("failed to get diffuse texture for material #{}", i)
                );
            }
        }
        else
        {
            diffuse_name = "white.png";
        }

        const auto diffuse_path = std::string("./assets/") + diffuse_name.C_Str();
        const auto diffuse = Texture::from_file_2d(diffuse_path);

        if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            if (material->GetTexture(aiTextureType_NORMALS, 0, &normal_name) != aiReturn_SUCCESS)
            {
                throw std::runtime_error(
                    fmt::format("failed to get normal texture for material #{}", i)
                );
            }
        }
        else
        {
            normal_name = "flat_normal.png";
        }

        const auto normal_path = std::string("./assets/") + normal_name.C_Str();
        const auto normal = Texture::from_file_2d(normal_path, false);

        m_materials.push_back(std::make_shared<Material>(diffuse, normal));
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
                .tangent = assimp_to_glm(mesh->mTangents[j]),
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

    m_bloom_program.attach_shader(GL_VERTEX_SHADER, "./shaders/postprocessing.vert.glsl");
    m_bloom_program.attach_shader(GL_FRAGMENT_SHADER, "./shaders/gaussian.frag.glsl");
    m_bloom_program.link();

    m_bloom_ping_pong_framebuffers[0].set_color_attachment(m_bloom_ping_pong_attachments[0]);
    m_bloom_ping_pong_framebuffers[1].set_color_attachment(m_bloom_ping_pong_attachments[1]);

    m_depth_program.attach_shader(GL_VERTEX_SHADER, "./shaders/depth.vert.glsl");
    m_depth_program.attach_shader(GL_FRAGMENT_SHADER, "./shaders/depth.frag.glsl");
    m_depth_program.link();

    m_shadow_map_framebuffer.set_depth_attachment(m_shadow_map_depth_attachment);
    m_shadow_map_framebuffer.set_draw_buffer(GL_NONE);
    m_shadow_map_framebuffer.set_read_buffer(GL_NONE);

    m_geometry_program.attach_shader(GL_VERTEX_SHADER, "./shaders/g_buffer.vert.glsl");
    m_geometry_program.attach_shader(GL_FRAGMENT_SHADER, "./shaders/g_buffer.frag.glsl");
    m_geometry_program.link();

    m_geometry_buffer.set_color_attachment(m_g_buffer_albedo, GL_COLOR_ATTACHMENT0);
    m_geometry_buffer.set_color_attachment(m_g_buffer_positions, GL_COLOR_ATTACHMENT1);
    m_geometry_buffer.set_color_attachment(m_g_buffer_normals, GL_COLOR_ATTACHMENT2);
    m_geometry_buffer.set_depth_attachment(m_g_buffer_depth);
    m_geometry_buffer.set_draw_buffers(
        std::array<GLenum, 3>{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2}
    );

    m_deferred_shading_program.attach_shader(GL_VERTEX_SHADER, "./shaders/deferred_shading.vert.glsl");
    m_deferred_shading_program.attach_shader(GL_FRAGMENT_SHADER, "./shaders/deferred_shading.frag.glsl");
    m_deferred_shading_program.link();

    m_post_processing_framebuffer.set_color_attachment(
        m_post_processing_color_attachment,
        GL_COLOR_ATTACHMENT0
    );
    m_post_processing_framebuffer.set_color_attachment(
        m_post_processing_color_attachment_bright,
        GL_COLOR_ATTACHMENT1
    );
    m_post_processing_framebuffer.set_depth_attachment(m_g_buffer_depth);
    m_post_processing_framebuffer.set_draw_buffers(
        std::array<GLenum, 2>{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1}
    );

    m_skybox_program.attach_shader(GL_VERTEX_SHADER, "./shaders/skybox.vert.glsl");
    m_skybox_program.attach_shader(GL_FRAGMENT_SHADER, "./shaders/skybox.frag.glsl");
    m_skybox_program.link();

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

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Geometry Buffer Render Pass");
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    m_geometry_buffer.bind();
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_geometry_program.use();
        m_geometry_program.set_uniform("view", m_camera.get_view_matrix());
        m_geometry_program.set_uniform("projection", m_camera.get_projection_matrix());
        m_geometry_program.set_uniform("camera_position", m_camera.m_eye);

        m_geometry_program.set_uniform("material.diffuse_map", 0);
        m_geometry_program.set_uniform("material.normal_map", 1);

        for (const auto &model : m_models)
        {
            model.draw(m_geometry_program);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPopDebugGroup();

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Deferred Shading Render Pass");
    m_post_processing_framebuffer.bind();
    {
        m_deferred_shading_program.use();
        m_deferred_shading_program.set_uniform("camera_position", m_camera.m_eye);
        m_deferred_shading_program.set_uniform("sun.direction", m_sun.get_direction());
        m_deferred_shading_program.set_uniform("sun.color", m_sun.m_color);
        m_deferred_shading_program.set_uniform("sun.ambient", m_sun.m_ambient);
        m_deferred_shading_program.set_uniform("sun.diffuse", m_sun.m_diffuse);
        m_deferred_shading_program.set_uniform("sun.specular", m_sun.m_specular);
        m_deferred_shading_program.set_uniform("sun.shadow_map", 0);
        m_deferred_shading_program.set_uniform("sun.transform", m_sun.get_light_space_matrix());
        m_deferred_shading_program.set_uniform("albedo_map", 1);
        m_deferred_shading_program.set_uniform("positions_map", 2);
        m_deferred_shading_program.set_uniform("normals_map", 3);
        m_shadow_map_depth_attachment.bind(GL_TEXTURE0);
        m_g_buffer_albedo.bind(GL_TEXTURE1);
        m_g_buffer_positions.bind(GL_TEXTURE2);
        m_g_buffer_normals.bind(GL_TEXTURE3);
        m_post_processing_plane.draw();

        // const auto camera_view = m_camera.get_view_matrix();
        // const auto camera_view_no_translation = glm::mat4(glm::mat3(camera_view));
        // glDepthFunc(GL_LEQUAL);
        // m_skybox_program.use();
        // m_skybox_program.set_uniform("view", camera_view_no_translation);
        // m_skybox_program.set_uniform("projection", m_camera.get_projection_matrix());
        // m_skybox_texture->bind(GL_TEXTURE0);
        // m_skybox_mesh.draw();
        // glDepthFunc(GL_LESS);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPopDebugGroup();

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Bloom Ping-Pong Render Pass");
    {
        auto horizontal = true;
        auto first_iteration = true;
        m_bloom_program.use();
        for (auto i = 0; i < 2 * m_bloom_amount; ++i)
        {
            m_bloom_ping_pong_framebuffers[static_cast<int>(horizontal)].bind();
            m_bloom_program.set_uniform("horizontal", horizontal);
            if (first_iteration)
            {
                m_post_processing_color_attachment_bright.bind(GL_TEXTURE0);
            }
            else
            {
                m_bloom_ping_pong_attachments[static_cast<int>(!horizontal)].bind(GL_TEXTURE0);
            }
            m_post_processing_plane.draw();
            horizontal = !horizontal;
            first_iteration = false;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPopDebugGroup();

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Post-Processing Render Pass");
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        m_post_processing_program.use();
        m_post_processing_program.set_uniform("gamma", m_gamma);
        m_post_processing_program.set_uniform("exposure", m_exposure);
        m_post_processing_program.set_uniform("screen_texture", 0);
        m_post_processing_program.set_uniform("bloom_texture", 1);
        m_post_processing_color_attachment.bind(GL_TEXTURE0);
        m_bloom_ping_pong_attachments[0].bind(GL_TEXTURE1);
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
        ImGui::SliderFloat3("Rotation", glm::value_ptr(m_sun.m_rotation), 0.0f, 359.999f);

        ImGui::SeparatorText("Color w/ Intensity");
        ImGui::ColorEdit3("Color", glm::value_ptr(m_sun.m_color));
        ImGui::ColorEdit3("Ambient", glm::value_ptr(m_sun.m_ambient));
        ImGui::SliderFloat("Diffuse", &m_sun.m_diffuse, 0.0f, 20.0f);
        ImGui::SliderFloat("Specular", &m_sun.m_specular, 0.0f, 1.0f);

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

    ImGui::Begin(
        "Post-Processing",
        nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize
    );
    {
        ImGui::SliderFloat("Gamma", &m_gamma, 0.0f, 3.0f);
        ImGui::SliderFloat("Exposure", &m_exposure, 0.0f, 10.0f);
        ImGui::SliderInt("Bloom Steps", &m_bloom_amount, 0, 10);
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
