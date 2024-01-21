#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Texture.h"

#include <glad/glad.h>

class Framebuffer
{
    GLuint m_framebuffer;

  public:
    explicit Framebuffer();
    Framebuffer(const Framebuffer &) = delete;
    const Framebuffer &operator=(const Framebuffer &) = delete;
    ~Framebuffer();

    void set_color_attachment(const Texture &texture);
    void set_depth_attachment(const Texture &texture);

    void set_draw_buffer(GLenum mode);
    void set_read_buffer(GLenum mode);

    void bind();

  private:
    void set_attachment(const Texture &texture, GLenum attachment);
};

#endif // FRAMEBUFFER_H
