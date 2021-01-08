//
// Created by klemen on 07/01/2021.
//

#ifndef TOWERDEFENSE_ENEMY_H
#define TOWERDEFENSE_ENEMY_H

#include "../../engine/graphics/Model.h"

struct EnemyConfig
{
    float speed = 10;
    float hp = 100;
    float ySpeed = 5;
    float rotationSpeed = 20;

};

class Terrain;

class Enemy
{
private:
    EnemyConfig config;

    Model model;
    ModelGroup *modelGroup{};

    glm::vec3 step{};
    glm::vec3 rotationSpeed{};

    float knockback = 0;

public:

    bool toRemove = false;

    explicit Enemy(const glm::vec3 &startPos, RawModel *rawModel, const EnemyConfig &enemyConfig,
                   const glm::vec3 &rotationSpeed);

    void update(float dt, float totalTime);

    bool dealDamage(float amount);

    bool hitGround(Terrain *terrain);

    const Model &getModel() const;

    Model *getModelPtr();

    ModelGroup *getModelGroup() const;

};

#endif //TOWERDEFENSE_ENEMY_H
