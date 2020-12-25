//
// Created by klemen on 19/12/2020.
//

#ifndef TOWERDEFENSE_MODEL_H
#define TOWERDEFENSE_MODEL_H

#include "RawModel.h"

class ModelGroup
{
public:
    // 109 bytes
    const Mesh *mesh; // 8B on 64-bit, 4B on 32-bit
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 modelMatrix;
    bool isStatic = false;

    ModelGroup(const Mesh *mesh = nullptr,
               const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
               const glm::vec3 &rotation = glm::vec3(0.0f, 0.0f, 0.0f),
               const glm::vec3 &scale = glm::vec3(1.0f, 1.0f, 1.0f)) :
            mesh(mesh), position(position), rotation(rotation),
            scale(scale)
    {}

    void makeStatic()
    {
        transform();
        isStatic = true;
    }

    void transform()
    {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        if (rotation.x != 0)
            modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        if (rotation.y != 0)
            modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        if (rotation.z != 0)
            modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        modelMatrix = glm::scale(modelMatrix, scale);
    }

};


/*
 * Wrapper for RawModel. Model class allows for
 * matrix transformations, RawModel just holds
 * the raw data.
 */
class Model
{
public:
    std::vector<ModelGroup> modelGroups;

    explicit Model(const RawModel &rawModel) : rawModel(rawModel)
    {
        for (auto &group:rawModel.groups)
            modelGroups.push_back(ModelGroup(&group.mesh));

    }
    const RawModel &rawModel;
};

#endif //TOWERDEFENSE_MODEL_H
