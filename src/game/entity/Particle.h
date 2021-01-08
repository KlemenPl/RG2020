//
// Created by klemen on 07/01/2021.
//

#ifndef TOWERDEFENSE_PARTICLE_H
#define TOWERDEFENSE_PARTICLE_H


#include "../../engine/graphics/Model.h"

class Particle
{
private:
    glm::vec3 step;
    glm::vec3 rotation;
    glm::vec3 scaleStep;
    float lifetime;

    Model model;
    ModelGroup *modelGroup{};

    float elapsedTime;

public:
    Particle(RawModel *rawModel, const glm::vec3 &startPos,const glm::vec3 &step, const glm::vec3 &rotation, const glm::vec3 &scaleStep,
             float lifetime);
    void update(float dt);
    bool finished();

    Model *getModelPtr();

    ModelGroup *getModelGroup() const;

};


#endif //TOWERDEFENSE_PARTICLE_H
