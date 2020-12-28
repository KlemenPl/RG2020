//
// Created by klemen on 26/12/2020.
//

#ifndef TOWERDEFENSE_TERRAIN_H
#define TOWERDEFENSE_TERRAIN_H

#include "Mesh.h"
#include "Color.h"


class Terrain
{
public:
    Mesh waterMesh;
    Mesh terrainMesh;

    float **heights;
    uint32_t heightsWidth;
    uint32_t heightsHeight; // ¯\_(ツ)_/¯

    Terrain()
    {}

    void generate(uint32_t xSize, uint32_t ySize,  float amplitude, uint32_t seed = 20, uint32_t resolution = 1);

    float getHeight(float xPos, float yPos);
    virtual ~Terrain();

};

#endif //TOWERDEFENSE_TERRAIN_H
