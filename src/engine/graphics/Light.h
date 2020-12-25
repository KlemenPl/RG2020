//
// Created by klemen on 23/12/2020.
//

#ifndef TOWERDEFENSE_LIGHT_H
#define TOWERDEFENSE_LIGHT_H

#include <glm.hpp>

struct Light
{
    glm::vec3 position;

    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;

    explicit Light(const glm::vec3 &position = {0.0f, 0.0f, 0.0f},
                   const glm::vec3 &ka = {0.0f, 0.0f, 0.0f}, // ambient colour
                   const glm::vec3 &kd = {0.0f, 0.0f, 0.0f}, // diffuse colour
                   const glm::vec3 &ks = {0.0f, 0.0f, 0.0f} // specular colour)
    ) : position(position), Ka(ka), Kd(kd), Ks(ks)
    {}

    bool operator==(const Light &rhs) const
    {
        return position == rhs.position &&
               Ka == rhs.Ka &&
               Kd == rhs.Kd &&
               Ks == rhs.Ks;
    }
    bool operator!=(const Light &rhs) const
    {
        return !(rhs == *this);
    }
};

struct DirLight
{
    static const uint32_t GLSL_SIZE = 16; // vec3 is padded as vec4 and must be multiple of vec4s

    glm::vec3 direction;

    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;

    DirLight(const glm::vec3 &direction = {0.0f, 0.0f, 0.0f},
             const glm::vec3 &ka = {1.0f, 1.0f, 1.0f}, // ambiance
             const glm::vec3 &kd = {1.0f, 1.0f, 1.0f}, // diffuse
             const glm::vec3 &ks = {1.0f, 1.0f, 1.0f}) // specular
            : direction(direction), Ka(ka), Kd(kd), Ks(ks)
    {}

    bool operator==(const DirLight &rhs) const
    {
        return direction == rhs.direction &&
               Ka == rhs.Ka &&
               Kd == rhs.Kd &&
               Ks == rhs.Ks;
    }
    bool operator!=(const DirLight &rhs) const
    {
        return !(rhs == *this);
    }
};

struct PointLight
{
    static const uint32_t GLSL_SIZE = 20; // vec3 is padded as vec4 and must be multiple of vec4s

    glm::vec3 position; // 4

    glm::vec3 Ka; // 4
    glm::vec3 Kd; // 4
    glm::vec3 Ks; // 4

    float constant;  // 1
    float linear;    // 1
    float quadratic; // 1
    // same as 4 vec4s

    PointLight(const glm::vec3 &position = {0.0f, 0.0f, 0.0f},
               const glm::vec3 &ka = {1.0f, 1.0f, 1.0f}, // ambiance
               const glm::vec3 &kd = {1.0f, 1.0f, 1.0f}, // diffuse
               const glm::vec3 &ks = {1.0f, 1.0f, 1.0f}, // specular
               float constant = 1.0f,
               float linear = 0.09f,
               float quadratic = 0.032f)
            : position(position), Ka(ka), Kd(kd), Ks(ks),
              constant(constant), linear(linear), quadratic(quadratic)
    {}

    bool operator==(const PointLight &rhs) const
    {
        return position == rhs.position &&
               constant == rhs.constant &&
               linear == rhs.linear &&
               quadratic == rhs.quadratic &&
               Ka == rhs.Ka &&
               Kd == rhs.Kd &&
               Ks == rhs.Ks;
    }
    bool operator!=(const PointLight &rhs) const
    {
        return !(rhs == *this);
    }
};

#endif //TOWERDEFENSE_LIGHT_H