//
// Created by klemen on 07/01/2021.
//

#include "Turret.h"
#include "Enemy.h"

Turret::Turret(const glm::vec3 pos, RawModel *rawModel)
        : model(*rawModel)
{
    modelMatrix = glm::translate(modelMatrix, pos);
    body = &model.modelGroups[0];
    head = &model.modelGroups[1];
}

void Turret::update(float dt, const std::deque<Enemy *> &enemies)
{
    float maxDst = 15.0f;
    float closest = maxDst;
    Enemy *closestEnemy = nullptr;

    for (auto &enemy:enemies)
    {
        float dst = glm::distance(head->position, enemy->getModelGroup()->position) < closest;
        if (dst < closest)
        {
            closest = dst;
            closestEnemy = enemy;
            if (closest < maxDst)
                break;
        }
    }

    if (closest < maxDst)
    {
        // "firing at enemy"
        closestEnemy->dealDamage(40);

    }

}
