//
// Created by klemen on 04/01/2021.
//

#include "CubeMap.h"
#include <string>
#include <stb_image.h>
#include <iostream>

CubeMap::CubeMap() :
        ID(0)
{}

CubeMap::~CubeMap()
{
    if (ID)
        numUses--;

    if (numUses == 0)
    {
        if (VBO)
            glDeleteBuffers(1, &VBO);
        if (IBO)
            glDeleteBuffers(1, &IBO);
        if (VAO)
            glDeleteVertexArrays(1, &VAO);
    }
}

void CubeMap::bind() const
{
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}
void CubeMap::unbind() const
{
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::loadCubeMap(std::vector<std::string> &faces)
{
    if (ID)
    {
        std::cout << "CubeMap::loadCubeMap() CubeMap already loaded." << std::endl;
        return;
    }

    numUses++;

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    int width, height, nrChannels;
    for (uint32_t i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            std::cout << "CubeMap::loadCubeMap() Could not load image: " << faces[i] << "." << std::endl;

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    if (!VAO)
    {
        // generating VAO, VBO
        uint32_t verticesLength = 108;
        float vertices[108] = {
                // positions
                -1.0f, 1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,

                -1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,

                -1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, 1.0f
        };


        uint32_t indiceCount = 0;
        for (int i = 0; i < verticesLength; i += 3)
        {
            bool flag = false;
            for (int j = 0; j < i; j += 3)
            {
                if (vertices[i] == vertices[j] &&
                    vertices[i + 1] == vertices[j + 1] &&
                    vertices[i + 2] == vertices[j + 2])
                {
                    flag = true;
                    break;
                }
            }
            std::cout<<vertices[i]<<", "<<vertices[i+1]<<", "<<vertices[i+2]<<", "<<std::endl;
            //std::cout<<indiceCount<<",";
            if(!flag)
                indiceCount++;
        }

        uint32_t indices[] = {0,1,2,3,3,4,4,5,5,5,5,6,6,6,7,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,};
        uint32_t indicesLength = 38;

        // VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(float), vertices, GL_STATIC_DRAW);

        // IBO
        //glGenBuffers(1, &IBO);
        //glBindBuffer(GL_ARRAY_BUFFER, IBO);
        //glBufferData(GL_ARRAY_BUFFER, indicesLength * sizeof(uint32_t), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    }
}