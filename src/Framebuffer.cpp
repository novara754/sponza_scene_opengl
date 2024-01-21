#include "Framebuffer.h"

#include <stdexcept>

Framebuffer::Framebuffer()
{
    glGenFramebuffers(1, &m_framebuffer);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &m_framebuffer);
}

void Framebuffer::set_color_attachment(const Texture &texture)
{
    set_attachment(texture, GL_COLOR_ATTACHMENT0);
}

void Framebuffer::set_depth_attachment(const Texture &texture)
{
    set_attachment(texture, GL_DEPTH_ATTACHMENT);
}

void Framebuffer::set_draw_buffer(const GLenum mode)
{
    // TODO - Use DSA
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glDrawBuffer(mode);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::set_read_buffer(const GLenum mode)
{
    // TODO - Use DSA
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glReadBuffer(mode);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind()
{
    if (glCheckNamedFramebufferStatus(m_framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("Framebuffer incomplete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

void Framebuffer::set_attachment(const Texture &texture, const GLenum attachment)
{
    // TODO - Use DSA
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.get_handle(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // glNamedFramebufferTexture(m_framebuffer, attachment, texture.get_handle(), 0);
}
