#include "Mesh.h"

#include <array>
#include <utility>

constexpr std::array PLANE_VERTICES = {
    Mesh::Vertex{{-1.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0}},
    Mesh::Vertex{{1.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0}},
    Mesh::Vertex{{1.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0}},
    Mesh::Vertex{{-1.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 1.0}},
    Mesh::Vertex{{-1.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0}},
};

constexpr std::array SKYBOX_VERTICES = {
    Mesh::Vertex{{-1.0f, 1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, -1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, -1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, -1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, 1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, 1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},

    Mesh::Vertex{{-1.0f, -1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, -1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, 1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, 1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, 1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, -1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},

    Mesh::Vertex{{1.0f, -1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, -1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, 1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, 1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, 1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, -1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},

    Mesh::Vertex{{-1.0f, -1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, 1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, 1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, 1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, -1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, -1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},

    Mesh::Vertex{{-1.0f, 1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, 1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, 1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, 1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, 1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, 1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},

    Mesh::Vertex{{-1.0f, -1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, -1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, -1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, -1.0f, -1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{-1.0f, -1.0f, 1.0f}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
    Mesh::Vertex{{1.0f, -1.0f, 1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0}}
};

Mesh Mesh::plane()
{
    return Mesh(PLANE_VERTICES);
}

Mesh Mesh::skybox()
{
    return Mesh(SKYBOX_VERTICES);
}

Mesh::Mesh(
    const std::span<const Vertex> vertices, const std::span<const std::uint32_t> indices,
    std::shared_ptr<Material> material
)
    : m_vertex_count(static_cast<GLsizei>(vertices.size())),
      m_index_count(static_cast<GLsizei>(indices.size())), m_material(std::move(material))
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
        vertices.data(),
        GL_STATIC_DRAW
    );

    if (m_index_count != 0)
    {
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(indices.size() * sizeof(std::uint32_t)),
            indices.data(),
            GL_STATIC_DRAW
        );
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void *>(offsetof(Vertex, position))
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void *>(offsetof(Vertex, normal))
    );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void *>(offsetof(Vertex, tex_coords))
    );

    glBindVertexArray(0);
}

void Mesh::draw() const
{
    if (m_material)
    {
        m_material->m_diffuse->bind(GL_TEXTURE0);
    }

    glBindVertexArray(m_vao);
    if (m_index_count != 0)
    {
        glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
    }
}
