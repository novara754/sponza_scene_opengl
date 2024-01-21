#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

class ShaderProgram
{
    GLuint m_program;
    std::vector<GLuint> m_shaders{};

  public:
    explicit ShaderProgram();
    ShaderProgram(const ShaderProgram &) = delete;
    const ShaderProgram &operator=(const ShaderProgram &) = delete;
    ~ShaderProgram();

    void attach_shader(GLenum shader_type, const std::string &filepath);
    void link();
    void use();

    void set_uniform(const std::string &name, int data);
    void set_uniform(const std::string &name, float data);
    void set_uniform(const std::string &name, const glm::vec3 &data);
    void set_uniform(const std::string &name, const glm::mat4 &data);
};

#endif // SHADER_PROGRAM_H
