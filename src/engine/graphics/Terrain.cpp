//
// Created by klemen on 27/12/2020.
//
#include "Terrain.h"
#include "../ResourceManager.h"
#include "../utils/Hash.h"
#include <gtc/noise.hpp>
#include <random>
#include <iostream>
#include <unordered_map>

// https://www.redblobgames.com/maps/terrain-from-noise/

struct TerrainVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    float colour;

    bool operator==(const TerrainVertex &rhs) const
    {
        return position == rhs.position &&
               normal == rhs.normal &&
               colour == rhs.colour;
    }

    struct HashFunction
    {
        size_t operator()(const TerrainVertex &key) const
        {
            size_t seed = 0;
            std::hash<float> hasher;
            Utils::hash_combine(seed, hasher(key.position.x));
            Utils::hash_combine(seed, hasher(key.position.y));
            Utils::hash_combine(seed, hasher(key.position.z));

            Utils::hash_combine(seed, hasher(key.normal.x));
            Utils::hash_combine(seed, hasher(key.normal.y));
            Utils::hash_combine(seed, hasher(key.normal.z));

            Utils::hash_combine(seed, hasher(key.colour));

            return seed;
        }
    };
};

struct WaterVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    float colour;
    float offset;

    bool operator==(const WaterVertex &rhs) const
    {
        return position == rhs.position &&
               normal == rhs.normal &&
               colour == rhs.colour &&
               offset == rhs.offset;
    }

    struct HashFunction
    {
        size_t operator()(const WaterVertex &key) const
        {
            //size_t rowHash = std::hash<int>()(pos.row);
            //size_t colHash = std::hash<int>()(pos.col) << 1;
            //return rowHash ^ colHash;

            uint32_t bitOffset = 0;

            size_t hash = std::hash<float>()(key.position.x) << ++bitOffset;
            hash ^= std::hash<float>()(key.position.y) << ++bitOffset;
            hash ^= std::hash<float>()(key.position.z) << ++bitOffset;

            hash ^= std::hash<float>()(key.normal.x) << ++bitOffset;
            hash ^= std::hash<float>()(key.normal.y) << ++bitOffset;
            hash ^= std::hash<float>()(key.normal.z) << ++bitOffset;

            hash ^= std::hash<float>()(key.colour) << ++bitOffset;
            hash ^= std::hash<float>()(key.offset) << ++bitOffset;


            return hash;
        }
    };

};

using IndicesHashMap = std::unordered_map<TerrainVertex, uint32_t, TerrainVertex::HashFunction>;

glm::vec3 calculateNormal(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3);

void processTerrainQuad(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &v4,
                        std::vector<TerrainVertex> &vertices,
                        std::vector<uint32_t> &indices,
                        IndicesHashMap &indicesMap, uint32_t *indicesIndex,
                        float minValue, float maxValue,
                        const Biome &biome);

void processTerrainTriangle(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, std::vector<TerrainVertex> &vertices,
                            std::vector<uint32_t> &indices,
                            IndicesHashMap &indicesMap, uint32_t *indicesIndex,
                            float minValue, float maxValue, const Biome &biome);

Terrain::Terrain()
{
    ResourceManager::loadTexture("res/textures/waterDuDvMap.png", "waterDuDvMap", false,
                                 GL_REPEAT, GL_REPEAT);
    waterDuDvMap = ResourceManager::getTexture2D("waterDuDvMap");

    ResourceManager::loadTexture("res/textures/waterMatchingNormalMap.png", "waterNormalMap", false,
                                 GL_REPEAT, GL_REPEAT);
    waterNormalMap = ResourceManager::getTexture2D("waterNormalMap");
}
void Terrain::generate(uint32_t xSize, uint32_t ySize, uint32_t detailX, uint32_t detailY,
                       uint32_t seed, uint32_t resolution, const Biome &biome)
{

    // 6051
    terrainMesh.dispose();
    waterMesh.dispose();

    std::mt19937 generator{seed};
    std::uniform_int_distribution<> random(-10000, 10000);

    if (resolution < 1) // TODO: LOD
        resolution = 1;

    if (xSize % resolution != 0 || ySize % resolution != 0)
    {
        std::cout << "Terrain::generate(): Invalid resolution." << std::endl;
        return;
    }

    //halfXSize = halfYSize = 5;

    uint32_t octaves = 4;
    glm::vec2 *octaveOffsets = new glm::vec2[octaves];

    for (uint32_t i = 0; i < octaves; i++)
    {
        float xOffset = random(generator);
        float yOffset = random(generator);
        octaveOffsets[i] = glm::vec2(xOffset, yOffset);
    }

    heightsWidth = xSize * detailX;
    heightsHeight = ySize * detailY;

    this->terrainXSize = (float) xSize;
    this->terrainYSize = (float) ySize;

    float halfXSize = xSize / 2.0f;
    float halfYSize = ySize / 2.0f;

    this->terrainHXSize = halfXSize;
    this->terrainHYSize = halfYSize;

    float stepX = (float) xSize / heightsWidth;
    float stepY = (float) ySize / heightsHeight;


    if (heights)
    {
        // freeing up memory
        for (int i = 0; i < heightsHeight; i++)
            delete[] heights[i];
        delete[] heights;
    }

    // allocating memory for perlin values
    heights = new float *[heightsHeight];
    for (int i = 0; i < heightsHeight; ++i)
        heights[i] = new float[heightsWidth];

    //float persistance = 1.5f;
    float persistance = biome.persistance;
    //float lacunarity = 1.35f;
    float lacunarity = biome.lacunarity;

    std::vector<glm::vec3> vertexPositions;

    this->minHeight = 0;
    this->maxHeight = 0 + biome.waterLevel;

    float flatMultiplier = 0.10f;

    for (uint32_t k = 0; k < biome.octaves; k++)
    {
        this->maxHeight += persistance;
    }

    /*
     * Applying fallout to corners to ensure
     * we always get an island.
     */
    //uint32_t centerX = std::round(heightsWidth / 2u);
    //uint32_t centerY = std::round(heightsHeight / 2u);
    //float maxDistance = (float) (heightsWidth * heightsHeight);

    uint32_t avgSize = (heightsWidth + heightsHeight) * 0.5;
    uint32_t falloff = avgSize * 0.55;
    uint32_t cutOff = avgSize * 0.1;


    for (uint32_t i = 0; i < heightsHeight; i++)
    {
        for (uint32_t j = 0; j < heightsWidth; j++)
        {
            float offsetX = j * stepX;
            float offsetY = i * stepY;

            float amplitude = biome.startAmplitude;
            float frequency = biome.startFrequency;
            float noiseHeight = 0.0f;

            // getting closest distance to border
            uint32_t minDst = avgSize;

            if (minDst > j)
                minDst = j;
            if (minDst > heightsWidth - j)
                minDst = heightsWidth - j;
            if (minDst > i)
                minDst = i;
            if (minDst > heightsHeight - i)
                minDst = heightsHeight - i;

            float multiplier = 1.0f;
            if (minDst < cutOff)
                multiplier = flatMultiplier;
            else if (minDst < falloff)
                multiplier = (float) (minDst - cutOff) / (falloff) + flatMultiplier;

            // calculating distance from center
            //float deltaX = (float) j - centerX;
            //float deltaY = (float) i - centerY;

            //float distance = std::abs(deltaX)+std::abs(deltaY);
            //float distance = std::sqrt(j * j + i * i);
            //float heightMultiplier = (maxDistance - distance) / maxDistance;
            //std::cout << distance << std::endl;
            //std::cout<<((maxDistance-distance)/maxDistance)<<std::endl;

            /*
            if (j < falloffX)
                noiseHeight = (float) -((falloffX - j) * stepX) / 2.0f;
            else if (j > heightsWidth - falloffX)
                noiseHeight = (float) -((-heightsWidth + j + falloffX) * stepX) / 2.0f;
            else if (i < falloffY)
                noiseHeight = (float) -((falloffY - i) * stepY) / 2.0f;
            else if (i > heightsHeight - falloffY)
                noiseHeight = (float) -((-heightsHeight + i + falloffY) * stepY) / 2.0f;
                */


            for (uint32_t k = 0; k < biome.octaves; k++)
            {
                float sampleX = (offsetX + octaveOffsets[k].x) * frequency;
                float sampleY = (offsetY + octaveOffsets[k].y) * frequency;

                float perlinValue = (glm::perlin(glm::vec2(sampleX, sampleY)) + 1) / 2.0f;
                //float perlinValue = (glm::perlin(glm::vec2(sampleX, sampleY)) / 2.0f) + 0.5f;
                //float perlinValue = (glm::perlin(glm::vec2(sampleX, sampleY)) / 2.0f) + 1.0f;
                noiseHeight += perlinValue * amplitude;

                amplitude *= persistance;
                frequency *= lacunarity;

            }
            //std::cout<<amplitude<<std::endl;

            float value = noiseHeight * multiplier;

            if (value > maxHeight)
                maxHeight = value;
            //else if (value < minHeight)
            //    minHeight = value;

            heights[i][j] = value;
            vertexPositions.emplace_back(offsetX - halfXSize, value, offsetY - halfYSize);
            //std::cout << heights[i][j] << '\t';

        }
        //std::cout << std::endl;
    }

    std::cout << "Terain::generate() Min: " << minHeight << ", Max: " << maxHeight << std::endl;
    delete[] octaveOffsets;

    std::vector<TerrainVertex> vertices;
    std::vector<uint32_t> indices;

    IndicesHashMap indicesMap;
    uint32_t indicesIndex = 0;

    for (int i = 0; i < heightsHeight - 1; i++)
    {
        for (int j = 0; j < heightsWidth - 1; j++)
        {
            uint32_t index1 = i * heightsHeight + j; // current
            uint32_t index2 = i * heightsHeight + (j + 1); // y + 0, x + 1
            uint32_t index3 = (i + 1) * heightsHeight + (j + 1); // y + 1, x + 1
            uint32_t index4 = (i + 1) * heightsHeight + (j + 0); // y + 1, x + 0

            // adding vertex data (sending in CCW)
            processTerrainQuad(vertexPositions[index2], vertexPositions[index1],
                               vertexPositions[index4], vertexPositions[index3],
                               vertices, indices, indicesMap, &indicesIndex,
                               minHeight, maxHeight, biome);
        }
    }

    // writing vertices into mesh
    terrainMesh.verticesLength = vertices.size() * 7;
    terrainMesh.vertices = nullptr;

    // generating terrain mesh
    glGenVertexArrays(1, &terrainMesh.VAO);
    glBindVertexArray(terrainMesh.VAO);

    glGenBuffers(1, &terrainMesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(TerrainVertex),
                          (const void *) offsetof(TerrainVertex, position));

    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex),
                          (const void *) offsetof(TerrainVertex, normal));

    glEnableVertexAttribArray(2); // colour
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TerrainVertex),
                          (const void *) offsetof(TerrainVertex, colour));

    glGenBuffers(1, &terrainMesh.IBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainMesh.IBO);
    terrainMesh.indicesLength = indices.size();
    terrainMesh.indices = nullptr;
    glBufferData(GL_ARRAY_BUFFER, terrainMesh.indicesLength * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

    /*
     * Generating water mesh
     */


    float yLevel = this->waterLevel;
    // @formatter:off
    /*
    float waterVertices[]{
             halfXSize, yLevel,  halfYSize,  1.0f, 1.0f, // top     right
            -halfXSize, yLevel,  halfYSize,  0.0f, 1.0f, // top     left
            -halfXSize, yLevel, -halfYSize,  0.0f, 0.0f, // bottom  left
             halfXSize, yLevel, -halfYSize,  1.0f, 0.0f, // bottom  right
    };
     */
    float xS=terrainXSize*2.0f;
    float yS=terrainYSize*2.0f;
    float waterVertices[]{
            xS, yLevel,  yS,  1.0f, 1.0f, // top     right
            -xS, yLevel,  yS,  0.0f, 1.0f, // top     left
            -xS, yLevel, -yS,  0.0f, 0.0f, // bottom  left
            xS, yLevel, -yS,  1.0f, 0.0f, // bottom  right
    };

    uint32_t waterIndices[]{
            2, 1, 0, // top left triangle
            0, 3, 2, // bottom right triangle
    };
    // @formatter:on

    waterMesh.verticesLength = 4 * 5;
    waterMesh.indicesLength = 6;
    waterMesh.vertices = nullptr;
    waterMesh.indices = nullptr;

    glGenVertexArrays(1, &waterMesh.VAO);
    glBindVertexArray(waterMesh.VAO);

    glGenBuffers(1, &waterMesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, waterMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, waterMesh.verticesLength * sizeof(float), waterVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void *) (3 * sizeof(float)));

    glGenBuffers(1, &waterMesh.IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterMesh.IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, waterMesh.indicesLength * sizeof(uint32_t), waterIndices, GL_STATIC_DRAW);

    std::uniform_real_distribution<float> terrainX(-halfXSize, halfXSize);
    std::uniform_real_distribution<float> terrainY(-halfYSize, halfYSize);
    std::uniform_int_distribution<uint32_t> object(0, 2);

    solid.clear();
    shrubs.clear();

    std::uniform_real_distribution<float> rotation(0.0f, 360.0f);
    std::uniform_real_distribution<float> scale(-0.1f, 0.1f);

    // spawning trees
    for (uint32_t i = 0; i < biome.treeFrequency; i++)
    {
        float posX = terrainX(generator);
        float posY = terrainY(generator);
        float height = getHeight(posX, posY);
        if (height > waterLevel + 0.5f && height < maxHeight - 5)
        {
            //Model tree = ResourceManager::getModel("tree_" + std::to_string(object(generator)));
            uint32_t treeVariant = object(generator);
            //treeVariant = 1;
            Ref<RawModel> rawTree = ResourceManager::getRawModel("tree_" + std::to_string(treeVariant));
            Model trunk = Model{*rawTree};
            trunk.disableGroup(1);
            Model leaves = Model{*rawTree};
            leaves.disableGroup(0);

            float treeRotation = rotation(generator);
            float treeScale = scale(generator);
            for (auto &group:trunk.modelGroups)
            {
                group.position.x = posX;
                group.position.z = posY;
                if (treeVariant == 0)
                {
                    group.position.y = height + 0.5f;
                }
                else if (treeVariant == 1)
                {
                    group.position.y = height + 1.5f;
                }
                else
                {
                    group.position.y = height + 1.0f;
                }

                group.scale.x = group.scale.y = group.scale.z = 0.35f + treeScale;
                group.rotation.y = treeRotation;

                group.makeStatic();
            }
            solid.emplace_back(trunk);

            for (auto &group:leaves.modelGroups)
            {
                group.position.x = posX;
                group.position.z = posY;
                if (treeVariant == 0)
                    group.position.y = height + 0.5f;
                else if (treeVariant == 1)
                    group.position.y = height + 1.5f;
                else
                    group.position.y = height + 1.0f;

                group.scale.x = group.scale.y = group.scale.z = 0.35f + treeScale;
                group.rotation.y = treeRotation;

                group.makeStatic();
            }
            shrubs.emplace_back(leaves);
        }
    }

    // spawning rocks
    for (uint32_t i = 0; i < biome.rockFrequency; i++)
    {
        float posX = terrainX(generator);
        float posY = terrainY(generator);
        float height = getHeight(posX, posY);
        if (height > 1.2f && height < maxHeight - 6)
        {
            //Model tree = ResourceManager::getModel("tree_" + std::to_string(object(generator)));
            uint32_t rockVariant = object(generator);
            Model rock = ResourceManager::getModel("rock_" + std::to_string(rockVariant));
            for (auto &group:rock.modelGroups)
            {
                group.position.x = posX;
                group.position.z = posY;
                group.position.y = height + 0.0f;

                group.scale.x = group.scale.y = group.scale.z = 0.25f + scale(generator);
                group.rotation.y = rotation(generator);

                group.makeStatic();
            }
            solid.emplace_back(rock);
        }
    }

    // spawning shrubs
    for (uint32_t i = 0; i < biome.shrubFrequency; i++)
    {
        float posX = terrainX(generator);
        float posY = terrainY(generator);
        float height = getHeight(posX, posY);
        if (height > 3.0f && height < maxHeight - 7)
        {
            //Model tree = ResourceManager::getModel("tree_" + std::to_string(object(generator)));
            uint32_t shrubVariant = object(generator);
            Model shrub = ResourceManager::getModel("shrub_" + std::to_string(shrubVariant));
            for (auto &group:shrub.modelGroups)
            {
                group.position.x = posX;
                group.position.z = posY;
                group.position.y = height + 0.25f;

                group.scale.x = group.scale.y = group.scale.z = 0.4f + scale(generator);
                group.rotation.y = rotation(generator);

                group.makeStatic();
            }
            shrubs.emplace_back(shrub);
        }
    }

    // spawning flowers
    for (uint32_t i = 0; i < biome.flowerFrequency; i++)
    {
        float posX = terrainX(generator);
        float posY = terrainY(generator);
        float height = getHeight(posX, posY);
        if (height > 3.0f && height < maxHeight - 7)
        {
            //Model tree = ResourceManager::getModel("tree_" + std::to_string(object(generator)));
            uint32_t flowerVariant = object(generator);
            Model flower = ResourceManager::getModel("flower_" + std::to_string(flowerVariant));
            for (auto &group:flower.modelGroups)
            {
                group.position.x = posX;
                group.position.z = posY;
                group.position.y = height;

                group.scale.x = group.scale.y = group.scale.z = 0.4f + scale(generator);
                group.rotation.y = rotation(generator);

                group.makeStatic();
            }
            shrubs.emplace_back(flower);
        }
    }

}

/*
 * CCW:
 * v1 - top right
 * v2 - top left
 * v3 - bottom left
 * v4 - bottom right
 */
void processTerrainQuad(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &v4,
                        std::vector<TerrainVertex> &vertices,
                        std::vector<uint32_t> &indices,
                        IndicesHashMap &indicesMap, uint32_t *indicesIndex,
                        float minValue, float maxValue, const Biome &biome)
{
    processTerrainTriangle(v1, v2, v3, vertices,
                           indices, indicesMap, indicesIndex,
                           minValue, maxValue, biome);
    processTerrainTriangle(v1, v3, v4, vertices,
                           indices, indicesMap, indicesIndex,
                           minValue, maxValue, biome);

}

template<typename T>
T clamp(T value, T lowerBound, T higherBound)
{
    return value < lowerBound ? lowerBound : value > higherBound ? higherBound : value;
}

float getColour(float height, float minValue, float maxValue, const Biome &biome)
{
    float value = (height - minValue) / maxValue;
    float deltaValue = value * (biome.colours.size() - 1);

    uint32_t cLower = std::floor(deltaValue);
    uint32_t cHigher = std::ceil(deltaValue);

    cLower = clamp<uint32_t>(cLower, 0u, biome.colours.size() - 1);
    cHigher = clamp<uint32_t>(cHigher, 0u, biome.colours.size() - 1);

    float colourInterpolation = deltaValue - ((uint32_t) deltaValue);

    Color interpolated = Color::interpolate(biome.colours[cLower], biome.colours[cHigher], colourInterpolation);
    return interpolated.pack();

}

void processTerrainVertex(glm::vec3 &v, glm::vec3 &normal, float color,
                          std::vector<TerrainVertex> &vertices,
                          std::vector<uint32_t> &indices,
                          IndicesHashMap &indicesMap, uint32_t *indicesIndex)
{
    TerrainVertex vertex{v, normal, color};

    auto it = indicesMap.find(vertex);

    if (it == indicesMap.end())
    {
        vertices.push_back(vertex);
        indices.push_back(*indicesIndex);
        indicesMap.insert(std::pair<TerrainVertex, uint32_t>(vertex, *indicesIndex));
        // indicesMap[vertex] = *indicesIndex; todo: on some seeds it messes up half of the terrain
    }
    else
        indices.push_back(it->second);

    (*indicesIndex)++;
}

void processTerrainTriangle(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, std::vector<TerrainVertex> &vertices,
                            std::vector<uint32_t> &indices,
                            IndicesHashMap &indicesMap, uint32_t *indicesIndex,
                            float minValue, float maxValue, const Biome &biome)
{

    // normal: x, y, z
    // calculating normal
    // https://stackoverflow.com/questions/1966587/given-3-points-how-do-i-calculate-the-normal-vector

    glm::vec3 normal = calculateNormal(v1, v2, v3);

    float avgHeight = (v1.y + v2.y + v3.y) / 3.0f;
    float color = getColour(avgHeight, minValue, maxValue, biome);
    //color = Colors::GREEN.pack();

    processTerrainVertex(v1, normal, color, vertices, indices, indicesMap, indicesIndex);
    processTerrainVertex(v2, normal, color, vertices, indices, indicesMap, indicesIndex);
    processTerrainVertex(v3, normal, color, vertices, indices, indicesMap, indicesIndex);

    //vertices.push_back({v1, normal, color});
    //vertices.push_back({v2, normal, color});
    //vertices.push_back({v3, normal, color});
}


Terrain::~Terrain()
{
    // freeing up memory
    for (int i = 0; i < heightsHeight; i++)
        delete[] heights[i];
    delete[] heights;

    terrainMesh.dispose();
    waterMesh.dispose();
}

float Terrain::getHeightFast(float xPos, float yPos) const
{
    xPos += terrainHXSize;
    yPos += terrainHYSize;

    if (xPos < 0 || xPos > terrainXSize ||
        yPos < 0 || yPos > terrainYSize)
        return -1.0f;

    float deltaX = xPos / terrainXSize;
    float deltaY = yPos / terrainYSize;
    uint32_t xIdx = std::round((heightsWidth - 1.0f) * deltaX);
    uint32_t yIdx = std::round((heightsHeight - 1.0f) * deltaY);
    return heights[yIdx][xIdx];
}

float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos)
{
    float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
    float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
    float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
    float l3 = 1.0f - l1 - l2;
    return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

float Terrain::getHeight(float xPos, float yPos)
{
    xPos += terrainHXSize;
    yPos += terrainHYSize;

    if (xPos < 0 || xPos > terrainXSize ||
        yPos < 0 || yPos > terrainYSize)
        return -1.0f;

    float grideSizeX = terrainXSize / ((float) heightsWidth - 1);
    float grideSizeY = terrainYSize / ((float) heightsHeight - 1);

    uint32_t gridX = std::floor(xPos / grideSizeX);
    uint32_t gridY = std::floor(yPos / grideSizeY);

    float xCoord = std::fmod(xPos, grideSizeX) / grideSizeX;
    float yCoord = std::fmod(yPos, grideSizeY) / grideSizeY;
    float answer;

    if (xCoord <= (1 - yCoord))
    {
        answer = barryCentric(glm::vec3(0, heights[gridY][gridX], 0),
                              glm::vec3(1, heights[gridY + 1][gridX], 0),
                              glm::vec3(0, heights[gridY][gridX + 1], 1),
                              glm::vec2(xCoord, yCoord));
    }
    else
    {
        answer = barryCentric(glm::vec3(1, heights[gridY + 1][gridX], 0),
                              glm::vec3(1, heights[gridY + 1][gridX + 1], 1),
                              glm::vec3(0, heights[gridY][gridX + 1], 1),
                              glm::vec2(xCoord, yCoord));
    }

    return answer;
}

glm::vec3 calculateNormal(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3)
{
    glm::vec3 dir = glm::cross(v2 - v1, v3 - v1);
    return glm::normalize(dir);
}
bool Biome::operator==(const Biome &rhs) const
{
    return octaves == rhs.octaves &&
           persistance == rhs.persistance &&
           lacunarity == rhs.lacunarity &&
           startAmplitude == rhs.startAmplitude &&
           startFrequency == rhs.startFrequency &&
           colours == rhs.colours &&
           waterLevel == rhs.waterLevel;
}
bool Biome::operator!=(const Biome &rhs) const
{
    return !(rhs == *this);
}
