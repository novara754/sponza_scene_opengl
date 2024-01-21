#ifndef MESH_H
#define MESH_H

#include <span>

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Mesh
{
  public:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coords;
    };

  private:
    GLsizei m_vertex_count{};
    GLsizei m_index_count{};
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_ebo{};

  public:
    [[nodiscard]] static Mesh plane();

    explicit Mesh(std::span<const Vertex> vertices, std::span<const std::uint32_t> indices);

    void draw() const;
};

#endif // MESH_H
