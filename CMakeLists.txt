cmake_minimum_required(VERSION 3.5)
project(sponza_scene)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(sponza_scene
        src/main.cpp
        src/App.cpp
        src/App.h
        src/Mesh.cpp
        src/Mesh.h
        src/ShaderProgram.cpp
        src/ShaderProgram.h
        src/Camera.h
        src/Texture.cpp
        src/Texture.h
        src/stb.cpp
        src/Camera.cpp
        src/PointLight.h
        src/Framebuffer.cpp
        src/Framebuffer.h
        src/Model.h
        src/DirectionalLight.h
)

target_compile_definitions(sponza_scene PRIVATE
        _CRT_SECURE_NO_WARNINGS
        GLFW_INCLUDE_NONE
        GLM_FORCE_EXPLICIT_CTOR
)

target_include_directories(sponza_scene PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(sponza_scene PRIVATE fmt::fmt)
target_link_libraries(sponza_scene PRIVATE spdlog::spdlog)
target_link_libraries(sponza_scene PRIVATE glad::glad)
target_link_libraries(sponza_scene PRIVATE glfw)
target_link_libraries(sponza_scene PRIVATE glm::glm)
target_link_libraries(sponza_scene PRIVATE imgui::imgui)
target_link_libraries(sponza_scene PRIVATE assimp::assimp)
