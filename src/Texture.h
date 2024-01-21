#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <span>
#include <string>

#include <glad/glad.h>

class Texture
{
    GLuint m_texture;
    GLenum m_target;

  public:
    static std::shared_ptr<Texture> from_file_2d(const std::string &filename);
    static std::shared_ptr<Texture> from_file_cubemap(std::span<const std::string> faces);
    static Texture color_attachment(int width, int height);
    static Texture depth_attachment(int width, int height);

    explicit Texture(GLuint texture, GLenum target);
    Texture(const Texture &) = delete;
    const Texture &operator=(const Texture &) = delete;

    ~Texture();

    void bind(GLenum slot);

    [[nodiscard]] GLuint get_handle() const;

  private:
    static Texture attachment(int width, int height, GLint internal_format, GLenum format);
};

#endif // TEXTURE_H
