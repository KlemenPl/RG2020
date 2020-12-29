//
// Created by klemen on 27/12/2020.
//
#include "Terrain.h"
#include "../Core.h"
#include <gtc/noise.hpp>
#include <random>
#include <iostream>
#include <unordered_map>

// https://www.redblobgames.com/maps/terrain-from-noise/

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    float colour;

    bool operator==(const Vertex &rhs) const
    {
        return position == rhs.position &&
               normal == rhs.normal &&
               colour == rhs.colour;
    }

    struct HashFunction
    {
        size_t operator()(const Vertex &key) const
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


            return hash;
        }
    };
};

glm::vec3 calculateNormal(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3);

void processQuad(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &v4, std::vector<Vertex> &vertices,
                 float minValue, float maxValue,
                 const Biome &biome);

void processTriangle(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, std::vector<Vertex> &vertices,
                     float minValue, float maxValue, const Biome &biome);

std::unordered_map<Vertex, uint32_t, Vertex::HashFunction> indicesMap;

void Terrain::generate(uint32_t xSize, uint32_t ySize, uint32_t seed, uint32_t resolution, const Biome &biome)
{

    terrainMesh.dispose();
    waterMesh.dispose();

    std::mt19937 generator{seed};
    std::uniform_int_distribution<> random(-100000, 100000);

    if (resolution < 1) // LOD
        resolution = 1;

    if (xSize % resolution != 0 || ySize % resolution != 0)
    {
        std::cout << "Terrain::generate(): Invalid resolution." << std::endl;
        return;
    }

    uint32_t halfXSize = xSize / 2;
    uint32_t halfYSize = ySize / 2;

    //halfXSize = halfYSize = 5;

    indicesMap.clear();

    uint32_t octaves = 4;
    glm::vec2 *octaveOffsets = new glm::vec2[octaves];

    for (uint32_t i = 0; i < octaves; i++)
    {
        float xOffset = random(generator);
        float yOffset = random(generator);
        octaveOffsets[i] = glm::vec2(xOffset, yOffset);
    }

    float divisor = 1.0f;

    heightsWidth = xSize / resolution;
    heightsHeight = ySize / resolution;

    float stepX = xSize / heightsWidth;
    float stepY = ySize / heightsHeight;


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

    this->minHeight = 100000.0f;
    this->maxHeight = -minHeight;

    for (uint32_t i = 0; i < heightsHeight; i++)
    {
        for (uint32_t j = 0; j < heightsWidth; j++)
        {

            float amplitude = biome.startAmplitude;
            float frequency = biome.startFrequency;
            float noiseHeight = biome.startNoiseHeight;

            for (uint32_t k = 0; k < biome.octaves; k++)
            {
                float sampleX = (j * stepX + octaveOffsets[k].x) / divisor * frequency;
                float sampleY = (i * stepY + octaveOffsets[k].y) / divisor * frequency;

                float perlinValue = (glm::perlin(glm::vec2(sampleX, sampleY)) + 1) / 2.0f;
                noiseHeight += perlinValue * amplitude;

                amplitude *= persistance;
                frequency *= lacunarity;

            }
            //std::cout<<amplitude<<std::endl;

            float value = noiseHeight;

            if (value > maxHeight)
                maxHeight = value;
            else if (value < minHeight)
                minHeight = value;

            heights[i][j] = value;
            vertexPositions.emplace_back(j * stepX - halfXSize, value, i * stepY - halfYSize);
            //std::cout << heights[i][j] << '\t';

        }
        //std::cout << std::endl;
    }

    std::cout << "Min: " << minHeight << ", Max: " << maxHeight << std::endl;
    delete[] octaveOffsets;

    std::vector<Vertex> vertices;

    for (int i = 0; i < heightsHeight - 1; i++)
    {
        for (int j = 0; j < heightsWidth - 1; j++)
        {
            uint32_t index1 = i * heightsHeight + j; // current
            uint32_t index2 = i * heightsHeight + (j + 1); // y + 0, x + 1
            uint32_t index3 = (i + 1) * heightsHeight + (j + 1); // y + 1, x + 1
            uint32_t index4 = (i + 1) * heightsHeight + (j + 0); // y + 1, x + 0

            // adding vertex data (sending in CCW)
            processQuad(vertexPositions[index2], vertexPositions[index1],
                        vertexPositions[index4], vertexPositions[index3],
                        vertices, minHeight, maxHeight, biome);
        }
    }

    // writing vertices into mesh
    uint32_t floatComponents = 7;
    terrainMesh.verticesLength = vertices.size() * floatComponents;
    terrainMesh.vertices = new float[terrainMesh.verticesLength];
    uint32_t idx = 0;
    for (int i = 0; i < vertices.size(); i++)
    {
        terrainMesh.vertices[idx + 0] = vertices[i].position.x;
        terrainMesh.vertices[idx + 1] = vertices[i].position.y;
        terrainMesh.vertices[idx + 2] = vertices[i].position.z;

        terrainMesh.vertices[idx + 3] = vertices[i].normal.x;
        terrainMesh.vertices[idx + 4] = vertices[i].normal.y;
        terrainMesh.vertices[idx + 5] = vertices[i].normal.z;

        terrainMesh.vertices[idx + 6] = vertices[i].colour;

        idx += floatComponents;

    }

    indicesMap.clear(); // 24192 vertices before using indices

    // generating mesh
    glGenVertexArrays(1, &terrainMesh.VAO);
    glBindVertexArray(terrainMesh.VAO);

    glGenBuffers(1, &terrainMesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, terrainMesh.verticesLength * sizeof(float), terrainMesh.vertices, GL_STATIC_DRAW);

    delete[] terrainMesh.vertices;
    terrainMesh.vertices = nullptr;

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (const void *) offsetof(Vertex, position));

    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2); // colour
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void *) offsetof(Vertex, colour));
}

/*
 * CCW:
 * v1 - top right
 * v2 - top left
 * v3 - bottom left
 * v4 - bottom right
 */
void processQuad(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &v4, std::vector<Vertex> &vertices,
                 float minValue, float maxValue, const Biome &biome)
{

    processTriangle(v1, v2, v3, vertices, minValue, maxValue, biome);
    processTriangle(v1, v3, v4, vertices, minValue, maxValue, biome);

}

template<typename T>
T clamp(T value, T lowerBound, T higherBound)
{
    return value < lowerBound ? lowerBound : value > higherBound ? higherBound : value;
}

float getColour(float height, float minValue, float maxValue, const Biome &biome)
{
    float value = (height - minValue) / maxValue;
    float deltaValue = value * (biome.colours.size() - 1.0f);

    uint32_t cLower = std::floor(deltaValue);
    uint32_t cHigher = std::ceil(deltaValue);

    cLower = clamp<uint32_t>(cLower, 0u, biome.colours.size() - 1);
    cHigher = clamp<uint32_t>(cHigher, 0u, biome.colours.size() - 1);

    float colorInterpolation = deltaValue - ((uint32_t)deltaValue);

    Color interpolated = Color::interpolate(biome.colours[cLower],biome.colours[cHigher],colorInterpolation);
    return interpolated.pack();

}

void processTriangle(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, std::vector<Vertex> &vertices,
                     float minValue, float maxValue, const Biome &biome)
{

    // normal: x, y, z
    // calculating normal
    // https://stackoverflow.com/questions/1966587/given-3-points-how-do-i-calculate-the-normal-vector

    glm::vec3 normal = calculateNormal(v1, v2, v3);

    float avgHeight = (v1.y + v2.y + v3.y) / 3.0f;
    float color = getColour(avgHeight, minValue, maxValue, biome);

    //color = Colors::GREEN.pack();

    vertices.push_back({v1, normal, color});
    vertices.push_back({v2, normal, color});
    vertices.push_back({v3, normal, color});
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

float Terrain::getHeight(float xPos, float yPos)
{
    return -1.0f;
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
           startNoiseHeight == rhs.startNoiseHeight &&
           heightBias == rhs.heightBias &&
           heightMultiplierBias == rhs.heightMultiplierBias &&
           colours == rhs.colours &&
           waterLevel == rhs.waterLevel;
}
bool Biome::operator!=(const Biome &rhs) const
{
    return !(rhs == *this);
}
