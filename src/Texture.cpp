#include "Texture.h"

#include <string>

#include <stb_image.h>
#include <stdexcept>

#include <fmt/format.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>
#include <spdlog/spdlog.h>

std::shared_ptr<Texture> Texture::from_file_2d(const std::string &filename)
{
    // TODO - Look into glTextureStorage (GL_ARB_texture_storage)
    // https://gamedev.stackexchange.com/questions/134177/whats-the-dsa-version-of-glteximage2d

    FILE *file = fopen(filename.c_str(), "rb");
    if (!file)
    {
        throw std::runtime_error(fmt::format("could not find file '{}'", filename));
    }
    int width, height, channels;
    auto *image_data = stbi_load_from_file(file, &width, &height, &channels, 0);

    GLint internal_format;
    GLenum format;
    if (channels == 4)
    {
        internal_format = GL_SRGB_ALPHA;
        format = GL_RGBA;
    }
    else if (channels == 3)
    {
        internal_format = GL_SRGB;
        format = GL_RGB;
    }
    else
    {
        throw std::runtime_error(
            fmt::format("incorrect number of channels ({}) on image '{}'", channels, filename)
        );
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internal_format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        image_data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image_data);
    fclose(file);

    return std::make_shared<Texture>(texture, GL_TEXTURE_2D);
}

std::shared_ptr<Texture> Texture::from_file_cubemap(
    std::span<const std::string> faces
)
{
    if (faces.size() != 6)
    {
        throw std::runtime_error("not enough faces");
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    for (auto i = 0; i < 6; ++i)
    {
        int width, height, channels;
        auto *image_data = stbi_load(faces[i].c_str(), &width, &height, &channels, 3);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return std::make_shared<Texture>(texture, GL_TEXTURE_CUBE_MAP);
}

Texture Texture::color_attachment(const int width, const int height)
{
    return attachment(width, height, GL_RGBA16F, GL_RGBA);
}

Texture Texture::depth_attachment(const int width, const int height)
{
    return attachment(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
}

Texture Texture::attachment(const int width, const int height, const GLint internal_format, const GLenum format)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internal_format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    constexpr auto border = glm::vec4(0.0, 0.0, 0.0, 1.0);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(border));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture(texture, GL_TEXTURE_2D);
}

void Texture::bind(const GLenum slot)
{
    glActiveTexture(slot);
    glBindTexture(m_target, m_texture);
}

GLuint Texture::get_handle() const
{
    return m_texture;
}

Texture::Texture(const GLuint texture, const GLenum target) : m_texture(texture), m_target(target)
{
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texture);
}
