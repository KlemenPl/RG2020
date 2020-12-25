//
// Created by klemen on 19/12/2020.
//

#ifndef TOWERDEFENSE_MATERIAL_H
#define TOWERDEFENSE_MATERIAL_H

#include <glm.hpp>

struct Material
{
public:

    static const uint32_t GLSL_SIZE = 16; // vec3 is padded as vec4 and must be multiple of vec4s

    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;
    float Ns;
    float Ni;

    explicit Material(const glm::vec3 &ka = {0.0f, 0.0f, 0.0f}, // ambient colour
                      const glm::vec3 &kd = {0.0f, 0.0f, 0.0f}, // diffuse colour
                      const glm::vec3 &ks = {0.0f, 0.0f, 0.0f}, // specular colour
                      float ns = 32.0f, // specular exponent 0 - 1000
                      float ni = 1.0f
    ) : Ka(ka), Kd(kd), Ks(ks), Ns(ns), Ni(ni)
    {}

    Material(const Material &material)
    = default;

    bool operator==(const Material &rhs) const
    {
        return Ka == rhs.Ka &&
               Kd == rhs.Kd &&
               Ks == rhs.Ks &&
               Ns == rhs.Ns;
    }
    bool operator!=(const Material &rhs) const
    {
        return !(rhs == *this);
    }

};

struct TexturedMaterial
{
public:

    static const uint32_t GLSL_SIZE = 16; // vec3 is padded as vec4 and must be multiple of vec4s

    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;
    float Ns;
    float Ni;

    explicit TexturedMaterial(const glm::vec3 &ka = {0.0f, 0.0f, 0.0f}, // ambient colour
                      const glm::vec3 &kd = {0.0f, 0.0f, 0.0f}, // diffuse colour
                      const glm::vec3 &ks = {0.0f, 0.0f, 0.0f}, // specular colour
                      float ns = 32.0f, // specular exponent 0 - 1000
                      float ni = 1.0f
    ) : Ka(ka), Kd(kd), Ks(ks), Ns(ns), Ni(ni)
    {}

    TexturedMaterial(const TexturedMaterial &material)
    = default;

    bool operator==(const TexturedMaterial &rhs) const
    {
        return Ka == rhs.Ka &&
               Kd == rhs.Kd &&
               Ks == rhs.Ks &&
               Ns == rhs.Ns;
    }
    bool operator!=(const TexturedMaterial &rhs) const
    {
        return !(rhs == *this);
    }

};

#endif //TOWERDEFENSE_MATERIAL_H
