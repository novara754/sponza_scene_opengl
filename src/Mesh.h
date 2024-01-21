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
    GLuint m_vao{};
    GLuint m_vbo{};

  public:
    [[nodiscard]] static Mesh cube();
    [[nodiscard]] static Mesh plane();

    explicit Mesh(std::span<const Vertex> vertices);
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;
    ~Mesh();

    void draw() const;
};

#endif // MESH_H
