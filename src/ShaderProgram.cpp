#include "ShaderProgram.h"

#include <fstream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram() : m_program(glCreateProgram())
{
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_program);
}

void ShaderProgram::attach_shader(GLenum shader_type, const std::string &filepath)
{
    std::ifstream shader_file(filepath);
    std::stringstream ss;
    ss << shader_file.rdbuf();
    std::string shader_src = ss.str();
    const auto *shader_src_c = shader_src.c_str();

    auto shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_src_c, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

        std::string log;
        log.resize(len);
        glGetShaderInfoLog(shader, len, nullptr, log.data());

        throw std::runtime_error{log};
    }

    glAttachShader(m_program, shader);
}

void ShaderProgram::link()
{
    glLinkProgram(m_program);

    GLint success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint len;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &len);

        std::string log;
        log.resize(len);
        glGetProgramInfoLog(m_program, len, nullptr, log.data());

        throw std::runtime_error{log};
    }

    for (const auto shader : m_shaders)
    {
        glDeleteShader(shader);
    }
}

void ShaderProgram::use()
{
    glUseProgram(m_program);
}

void ShaderProgram::set_uniform(const std::string &name, const int data)
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), data);
}

void ShaderProgram::set_uniform(const std::string &name, const float data)
{
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), data);
}

void ShaderProgram::set_uniform(const std::string &name, const glm::vec3 &data)
{
    glUniform3f(glGetUniformLocation(m_program, name.c_str()), data.x, data.y, data.z);
}

void ShaderProgram::set_uniform(const std::string &name, const glm::mat4 &data)
{
    glUniformMatrix4fv(
        glGetUniformLocation(m_program, name.c_str()),
        1,
        GL_FALSE,
        glm::value_ptr(data)
    );
}
