cmake_minimum_required(VERSION 3.5)
project(sponza_scene)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SPDLOG_FMT_EXTERNAL ON)

set(SDL_STATIC ON)
set(SDL_SHARED OFF)

set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_GLTF_IMPORTER ON)

FetchContent_Declare(
        fmt
        SYSTEM
        GIT_REPOSITORY "https://github.com/fmtlib/fmt"
        GIT_TAG "11.0.2"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        spdlog
        SYSTEM
        GIT_REPOSITORY "https://github.com/gabime/spdlog"
        GIT_TAG "v1.15.0"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
        assimp
        SYSTEM
        GIT_REPOSITORY "https://github.com/assimp/assimp"
        GIT_TAG "v5.4.3"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(assimp)

FetchContent_Declare(
        stb
        SYSTEM
        GIT_REPOSITORY "https://github.com/nothings/stb"
        GIT_TAG "2e2bef463a5b53ddf8bb788e25da6b8506314c08"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
        glm
        SYSTEM
        GIT_REPOSITORY "https://github.com/g-truc/glm"
        GIT_TAG "1.0.1"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
        imgui
        SYSTEM
        GIT_REPOSITORY "https://github.com/ocornut/imgui"
        GIT_TAG "71c77c081ac36841e682498229088e7678207112"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
        glfw
        SYSTEM
        GIT_REPOSITORY "https://github.com/glfw/glfw"
        GIT_TAG "3.4"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(glfw)

add_subdirectory(glad)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp

        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp

        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PRIVATE glfw)

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

target_include_directories(sponza_scene PRIVATE ${stb_SOURCE_DIR})
target_link_libraries(sponza_scene PRIVATE spdlog::spdlog)
target_link_libraries(sponza_scene PRIVATE glad)
target_link_libraries(sponza_scene PRIVATE glfw)
target_link_libraries(sponza_scene PRIVATE glm::glm)
target_link_libraries(sponza_scene PRIVATE imgui)
target_link_libraries(sponza_scene PRIVATE assimp::assimp)
