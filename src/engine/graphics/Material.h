//
// Created by klemen on 19/12/2020.
//

#ifndef TOWERDEFENSE_MATERIAL_H
#define TOWERDEFENSE_MATERIAL_H

#include <glm.hpp>

struct Material
{
public:
    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;
    float Ns;

    explicit Material(const glm::vec3 &ka = {1.0f, 1.0f, 1.0f}, // ambient colour
                      const glm::vec3 &kd = {1.0f, 1.0f, 1.0f}, // diffuse colour
                      const glm::vec3 &ks = {0.0f, 0.0f, 0.0f}, // specular colour
                      float ns = 100.0f // specular exponent 0 - 1000
    ) : Ka(ka), Kd(kd), Ks(ks), Ns(ns)
    {}

    Material(const Material &material)
    = default;


};

#endif //TOWERDEFENSE_MATERIAL_H
