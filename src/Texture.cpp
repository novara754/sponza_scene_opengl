#include "Texture.h"

#include <string>

#include <stb_image.h>
#include <stdexcept>

#include <fmt/format.h>

Texture Texture::from_file_2d(const std::string &filename)
{
    // TODO - Look into glTextureStorage (GL_ARB_texture_storage)
    // https://gamedev.stackexchange.com/questions/134177/whats-the-dsa-version-of-glteximage2d

    FILE *file = fopen(filename.c_str(), "rb");
    if (!file)
    {
        throw std::runtime_error(fmt::format("could not find file '{}'", filename));
    }
    int width, height, channels;
    auto *image_data = stbi_load_from_file(file, &width, &height, &channels, 4);

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
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image_data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image_data);
    fclose(file);

    return Texture(texture, GL_TEXTURE_2D);
}

Texture Texture::color_attachment(const int width, const int height)
{
    return attachment(width, height, GL_RGBA);
}

Texture Texture::depth_attachment(const int width, const int height)
{
    return attachment(width, height, GL_DEPTH_COMPONENT);
}

Texture Texture::attachment(const int width, const int height, const GLenum format)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        static_cast<GLint>(format),
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
