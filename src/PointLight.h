#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <glm/glm.hpp>

struct PointLight
{
    glm::vec3 m_position;
    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    float m_constant_attenuation;
    float m_linear_attenuation;
    float m_quadratic_attenuation;
};

#endif // POINTLIGHT_H
