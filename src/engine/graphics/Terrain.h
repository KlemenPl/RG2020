//
// Created by klemen on 26/12/2020.
//

#ifndef TOWERDEFENSE_TERRAIN_H
#define TOWERDEFENSE_TERRAIN_H

#include "Mesh.h"
#include "Color.h"
#include "Model.h"
#include "Texture2D.h"
#include <vector>

struct Biome
{
    uint32_t octaves = 3;

    float persistance = 3.282f;
    float lacunarity = 0.282f;

    float startAmplitude = 1.0f;
    float startFrequency = 1.0f;

    std::vector<Color> colours{};

    float waterLevel = 1.0f;

    bool operator==(const Biome &rhs) const;
    bool operator!=(const Biome &rhs) const;

};


class Terrain
{
private:
    Model *terrainModels = nullptr;
    uint32_t terrianModelsLength = 0;
public:
    Mesh waterMesh;
    Mesh terrainMesh;

    float **heights = nullptr; // pointer to pointer for 2D array
    uint32_t heightsWidth;
    uint32_t heightsHeight; // ¯\_(ツ)_/¯

    float minHeight;
    float maxHeight;

    float waterLevel = 0;

    std::vector<Model> trees;
    std::vector<Model> shrubs;
    std::vector<Model> rocks;

    Ref<Texture2D> waterDuDvMap;
    Ref<Texture2D> waterNormalMap;

    Terrain();

    void generate(uint32_t xSize, uint32_t ySize, uint32_t detailX, uint32_t detailY, uint32_t seed = 20,
                  uint32_t resolution = 1.0f, const Biome &biome = {});

    float getHeight(float xPos, float yPos);
    virtual ~Terrain();

};

#endif //TOWERDEFENSE_TERRAIN_H
