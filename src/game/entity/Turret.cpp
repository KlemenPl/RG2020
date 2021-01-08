//
// Created by klemen on 07/01/2021.
//

#include "Turret.h"
#include "Enemy.h"

Turret::Turret(const glm::vec3 &pos, RawModel *rawModel, float fireSpeed, float damage, float range)
        : model(*rawModel), fireSpeed(fireSpeed), damage(damage), range(range)
{
    model.setTranslation(pos);
    body = &model.modelGroups[0];
    head = &model.modelGroups[1];
}

void Turret::lookAt(const glm::vec3 &pos)
{
    glm::vec3 direction = glm::normalize(pos - head->position);
    head->rotation.x = -glm::degrees(asinf(direction.y));
    head->rotation.y = glm::degrees(atan2f(direction.x, direction.z));
}

void Turret::update(float dt, const std::deque<Enemy *> &enemies)
{
    elapsedTime += dt;

    closestEnemy = nullptr;

    if (elapsedTime < fireSpeed)
        return;

    float maxDst = 50.0f;
    float closest = maxDst;

    for (auto &enemy:enemies)
    {
        float dst = glm::distance(head->position, enemy->getModelGroup()->position);
        if (dst < closest)
        {
            closest = dst;
            closestEnemy = enemy;
        }
    }
    if (closestEnemy != nullptr)
        lookAt(closestEnemy->getModelGroup()->position);
}

Enemy *Turret::shoot()
{
    if (closestEnemy == nullptr || elapsedTime < fireSpeed)
        return nullptr;

    elapsedTime = 0;
    return closestEnemy;
}

Model *Turret::getModelPtr()
{
    return &model;
}
float Turret::getFireSpeed() const
{
    return fireSpeed;
}
float Turret::getDamage() const
{
    return damage;
}
