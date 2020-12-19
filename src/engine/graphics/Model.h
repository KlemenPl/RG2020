//
// Created by klemen on 19/12/2020.
//

#ifndef TOWERDEFENSE_MODEL_H
#define TOWERDEFENSE_MODEL_H

#include "RawModel.h"

struct ModelMesh
{
    const Mesh &mesh;
    Material material;
};

struct ModelGroup
{
};

/*
 * Wrapper for RawModel. Model class allows for
 * matrix transformations, RawModel just holds
 * vertex data.
 */
class Model
{
private:
    const RawModel &rawModel;

    int numGroups;
    ModelGroup *groups;
public:


    explicit Model(const RawModel &rawModel) : rawModel(rawModel)
    {

    }
};

#endif //TOWERDEFENSE_MODEL_H
