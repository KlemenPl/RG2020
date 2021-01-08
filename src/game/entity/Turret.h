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

    Enemy *closestEnemy = nullptr;

    float fireSpeed = 0.0f;
    float elapsedTime = 0.0f;
    float range=0.0f;
    float damage = 0.0f;

public:
    explicit Turret(const glm::vec3& pos, RawModel *rawModel, float fireSpeed, float damage, float range);

    void update(float dt, const std::deque<Enemy *> &enemies);
    Enemy* shoot();

    Model *getModelPtr();

    void lookAt(const glm::vec3 &pos);

    float getFireSpeed() const;
    float getDamage() const;

};


#endif //TOWERDEFENSE_TURRET_H
