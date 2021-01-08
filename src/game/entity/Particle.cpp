//
// Created by klemen on 07/01/2021.
//

#include "Particle.h"

Particle::Particle(RawModel *rawModel, const glm::vec3 &startPos, const glm::vec3 &step,
                   const glm::vec3 &rotation, const glm::vec3 &scaleStep, float lifetime) :
        model(*rawModel),
        step(step), rotation(rotation),
        scaleStep(scaleStep), lifetime(lifetime), elapsedTime(0)
{

    modelGroup = &model.modelGroups.front();
    modelGroup->position = startPos;
}

void Particle::update(float dt)
{
    elapsedTime += dt;

    modelGroup->position += step * dt;
    modelGroup->rotation += rotation * dt;
    modelGroup->scale += scaleStep * dt;
}
bool Particle::finished()
{
    return elapsedTime >= lifetime;
}

Model *Particle::getModelPtr()
{
    return &model;
}
ModelGroup *Particle::getModelGroup() const
{
    return modelGroup;
}

