//
// Created by klemen on 26/12/2020.
//

#ifndef TOWERDEFENSE_TERRAIN_H
#define TOWERDEFENSE_TERRAIN_H

#include "Mesh.h"
#include "Color.h"
#include "Model.h"

struct Biome
{
    uint32_t octaves = 4;

    float persistance = 1.5f;
    float lacunarity = 0.35f;

    float startNoiseHeight = -4.0f;

    float startAmplitude = 2.0f;
    float startFrequency = 1.0f;


    float heightBias = 1.0f;
    float heightMultiplierBias = 0.5f;

    std::vector<Color> colours{};

    float waterLevel = 0.0f;

    bool operator==(const Biome &rhs) const;
    bool operator!=(const Biome &rhs) const;

};


class Terrain
{
private:
    Model* terrainModels = nullptr;
    uint32_t terrianModelsLength = 0;
public:
    Mesh waterMesh;
    Mesh terrainMesh;

    float **heights = nullptr;
    uint32_t heightsWidth;
    uint32_t heightsHeight; // ¯\_(ツ)_/¯

    float minHeight;
    float maxHeight;

    Terrain()
    {}

    void generate(uint32_t xSize, uint32_t ySize, uint32_t seed = 20, uint32_t resolution = 1.0f, const Biome& biome={});

    float getHeight(float xPos, float yPos);
    virtual ~Terrain();

};

#endif //TOWERDEFENSE_TERRAIN_H
