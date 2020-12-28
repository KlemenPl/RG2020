//
// Created by klemen on 27/12/2020.
//
#include "Terrain.h"
#include "../Core.h"
#include <gtc/noise.hpp>
#include <random>
#include <iostream>

// https://www.redblobgames.com/maps/terrain-from-noise/

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    float colour;
};

glm::vec3 calculateNormal(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3);

void processQuad(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &v4, std::vector<Vertex> &vertices);

void processTriangle(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, std::vector<Vertex> &vertices);

float getColour(float height);

void Terrain::generate(uint32_t xSize, uint32_t ySize, float amplitude, uint32_t seed, uint32_t resolution)
{

    uint32_t totalLength = xSize * ySize;

    terrainMesh.dispose();
    waterMesh.dispose();

    std::mt19937 random{seed};

    uint32_t xOffset = (random() % 2000) - 1000;
    uint32_t yOffset = (random() % 2000) - 1000;

    srand(seed);

    float divisor = 50.0f;
    float rng = ((xOffset*random())%yOffset)/divisor;

    heightsWidth = xSize / resolution;
    heightsHeight = ySize / resolution;

    float stepX = xSize / heightsWidth;
    float stepY = ySize / heightsHeight;


    // allocating memory for perlin values
    heights = new float *[heightsHeight];
    for (int i = 0; i < heightsHeight; ++i)
        heights[i] = new float[heightsWidth];



    std::vector<glm::vec3> vertexPositions;

    for (uint32_t i = 0; i < heightsHeight; i++)
    {
        for (uint32_t j = 0; j < heightsWidth; j++)
        {

            float sampleX = (xOffset + j * stepX) / divisor;
            float sampleY = (yOffset + i * stepY) / divisor;

            float value = glm::perlin(glm::vec3(sampleX, sampleY,rng)) * amplitude;
            heights[i][j] = value;

            vertexPositions.emplace_back(j * stepX,  value,i * stepY);
            std::cout << heights[i][j] << '\t';

        }
        std::cout << std::endl;
    }

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
                        vertices);
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

    // generating mesh
    glGenVertexArrays(1, &terrainMesh.VAO);
    glBindVertexArray(terrainMesh.VAO);

    glGenBuffers(1, &terrainMesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, terrainMesh.verticesLength * sizeof(float), terrainMesh.vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (const void *) offsetof(Vertex, position));

    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (const void *) offsetof(Vertex, normal));

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
void processQuad(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &v4, std::vector<Vertex> &vertices)
{

    processTriangle(v1, v2, v3, vertices);
    processTriangle(v1, v3, v4, vertices);

}

void processTriangle(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, std::vector<Vertex> &vertices)
{

    // normal: x, y, z
    // calculating normal
    // https://stackoverflow.com/questions/1966587/given-3-points-how-do-i-calculate-the-normal-vector

    glm::vec3 normal = calculateNormal(v1, v2, v3);
    float color = Colors::BLUE.pack();

    vertices.push_back({v1, normal, color});
    vertices.push_back({v2, normal, color});
    vertices.push_back({v3, normal, color});
}

float getColour(float height)
{
    Color c{0.0f, 0.0f, 0.0f};
    Color c1{0.0f, 0.0f, 0.0f};
    Color c2{0.0f, 0.0f, 0.0f};

    static Color hC1{183.0f / 255.0f, 148.0f / 255.0f, 151.0f / 255.0f};
    static Color hC2{80.0f / 255.0f, 74.0f / 255.0f, 66.0f / 255.0f};
    static Color hC3{56.0f / 255.0f, 82.0f / 255.0f, 52.0f / 255.0f};
    static Color hC4{92.0f / 255.0f, 149.0f / 255.0f, 80.0f / 255.0f};
    static Color hC5{145.0f / 255.0f, 155.0f / 255.0f, 69.0f / 255.0f};

    return c.pack();
}

Terrain::~Terrain()
{
    // freeing up memory
    for (int i = 0; i < heightsHeight; i++)
        delete[] heights[i];
    delete[] heights;
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
