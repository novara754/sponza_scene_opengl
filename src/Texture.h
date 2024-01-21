#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include <glad/glad.h>

class Texture
{
    GLuint m_texture;
    GLenum m_target;

  public:
    static Texture from_file_2d(const std::string &filename);
    static Texture color_attachment(int width, int height);
    static Texture depth_attachment(int width, int height);

    Texture(const Texture &) = delete;
    const Texture &operator=(const Texture &) = delete;

    ~Texture();

    void bind(GLenum slot);

    [[nodiscard]] GLuint get_handle() const;

  private:
    static Texture attachment(int width, int height, GLenum format);

    explicit Texture(GLuint texture, GLenum target);
};

#endif // TEXTURE_H
