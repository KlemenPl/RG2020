//
// Created by klemen on 20/12/2020.
//

#ifndef TOWERDEFENSE_BOUNDINGBOX3D_H
#define TOWERDEFENSE_BOUNDINGBOX3D_H

#include <glm.hpp>
#include "graphics/Model.h"

class BoundingBox3D
{
    Model* model{};
    glm::vec3 points[8]{};
};

#endif //TOWERDEFENSE_BOUNDINGBOX3D_H
