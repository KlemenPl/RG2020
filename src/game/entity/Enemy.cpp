//
// Created by klemen on 07/01/2021.
//

#include "Enemy.h"
#include "../../engine/graphics/Terrain.h"

Enemy::Enemy(const glm::vec3 &startPos, RawModel *rawModel, const EnemyConfig &enemyConfig,
             const glm::vec3 &rotationSpeed)
        : model(*rawModel), config(enemyConfig), rotationSpeed(rotationSpeed)
{
    modelGroup = &model.modelGroups.front();
    modelGroup->position = startPos;
    modelGroup->scale = glm::vec3(1.2f, 1.2f, 1.2f);
    // calculating step
    step = glm::vec3(0, 0, 0) - startPos;
    step *= 0.005f;
}

void Enemy::update(float dt, float totalTime)
{
    float posY = modelGroup->position.y + sinf(totalTime * config.ySpeed) * dt;
    if (knockback <= 0)
    {
        modelGroup->position += step * dt * config.speed;
        modelGroup->position.y = posY;
        modelGroup->rotation += rotationSpeed * dt * rotationSpeed;
    }
    else
    {
        modelGroup->position += -(knockback) * step * dt * config.speed;
        modelGroup->position.y = posY;
        modelGroup->rotation += rotationSpeed * dt * rotationSpeed;
        knockback -= dt;
    }
}

bool Enemy::hitGround(Terrain *terrain)
{
    float height = terrain->getHeight(modelGroup->position.x, modelGroup->position.z);

    return modelGroup->position.y < height ||
           std::abs(modelGroup->position.x) < 1.0f && std::abs(modelGroup->position.z) < 1.0f;
}

bool Enemy::dealDamage(float amount)
{
    knockback += amount / 80.0f;
    config.hp -= amount;
    return config.hp <= 0.0f;
}

const Model &Enemy::getModel() const
{
    return model;
}
Model *Enemy::getModelPtr()
{
    return &model;
}
ModelGroup *Enemy::getModelGroup() const
{
    return modelGroup;
}


