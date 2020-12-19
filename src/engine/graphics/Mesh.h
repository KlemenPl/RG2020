//
// Created by klemen on 19/12/2020.
//

#ifndef TOWERDEFENSE_MESH_H
#define TOWERDEFENSE_MESH_H

#include "Material.h"

class Mesh
{
public:
    Material defaultMaterial;
    uint32_t VAO = 0;
    uint32_t VBO = 0;
    uint32_t IBO = 0;

    float *vertices = nullptr;
    uint32_t *indices = nullptr;

    uint32_t verticesLength = 0;
    uint32_t indicesLength = 0;

    bool hasNormals = false;
    bool hasTexCoords = false;

    Mesh() = default;

    void bind() const;
    void unbind() const;
};

#endif //TOWERDEFENSE_MESH_H
