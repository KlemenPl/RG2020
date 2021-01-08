//
// Created by klemen on 07/01/2021.
//

#ifndef TOWERDEFENSE_TURRET_H
#define TOWERDEFENSE_TURRET_H

#include <deque>
#include "../../engine/graphics/Model.h"

class Enemy;

class Turret
{
private:
    Model model;
    ModelGroup *body;
    ModelGroup *head;

    glm::mat4 modelMatrix;

public:
    explicit Turret(const glm::vec3 pos, RawModel *rawModel);

    void update(float dt, const std::deque<Enemy *> &enemies);

};


#endif //TOWERDEFENSE_TURRET_H
