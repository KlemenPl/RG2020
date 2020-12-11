//
// Created by klemen on 06/12/2020.
//

#include <iostream>
#include "Renderer2D.h"
#include "RenderingCapabilities.h"
#include "../graphics/Vertex2D.h"
#include "../ResourceManager.h"


Renderer2D::Renderer2D(const Shader shader) : shader(shader)
{
    Texture2D texture2D = ResourceManager::getTexture2D("rabbit");
    Color color = Color(0.0, 0.0, 1.0, 1.0);
    float packed = color.pack();
    // configure VAO/VBO
    float textureID = static_cast<float>(texture2D.ID);
    /*
    float vertices[] = {
            // pos      // tex
            0.0f, 0.0f, 0.0f, 0.0f, packed, textureID, // bottom left
            0.0f, 1.0f, 0.0f, 1.0f, packed, textureID, // top left
            //1.0f, 0.0f, 1.0f, 0.0f, packed, textureID,

            //0.0f, 1.0f, 0.0f, 1.0f, packed, textureID,
            1.0f, 1.0f, 1.0f, 1.0f, packed, textureID, // top right
            1.0f, 0.0f, 1.0f, 0.0f, packed, textureID // bottom right
    };*/

    this->maxTextures = RenderingCapabilities::MAX_TEXTURE_IMAGE_UNITS;
    this->sampledTextures = new int[maxTextures];
    this->drawing=false;
    this->drawCalls=0;

    this->quadsPerBatch = 1000; // 4k sprites per batch
    this->indicesSize = quadsPerBatch * 6;
    this->vertexSize = sizeof(Vertex2D);
    this->verticesSize = quadsPerBatch * 4 * vertexSize;

    this->vertices = new float[verticesSize];
    for (int i = 0; i < verticesSize; i++)
        vertices[i] = 0;

    this->indices = new unsigned int[indicesSize];
    for (int i = 0, vboOffset = 0; i < indicesSize; i += 6, vboOffset += 4)
    {
        indices[i + 0] = vboOffset + 0;
        indices[i + 1] = vboOffset + 1;
        indices[i + 2] = vboOffset + 2;

        indices[i + 3] = vboOffset + 0;
        indices[i + 4] = vboOffset + 2;
        indices[i + 5] = vboOffset + 3;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(float), indices, GL_STATIC_DRAW);


    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, pos));

    glEnableVertexAttribArray(1); // texture position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, texCoords));

    glEnableVertexAttribArray(2); // colour
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, color));

    glEnableVertexAttribArray(3); // texture ID
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_TRUE, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, textureID));



    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);
    //glActiveTexture(GL_TEXTURE0);
    //texture2D.bind();

    //this->shader.setUniform("textures",1,1);

}


Renderer2D::~Renderer2D()
{
    glDeleteBuffers(GL_ARRAY_BUFFER, &VBO);
    glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &IBO);
    glDeleteVertexArrays(1, &VAO);

    delete[] indices;
    delete[] vertices;
    delete[] sampledTextures;
}


void Renderer2D::begin()
{
    if (drawing)
        throw std::runtime_error("Renderer2D::begin(): Cannot begin batch while it is already drawing");

    drawing = true;
    drawOffset = 0;
    drawElements = 0;
    textureIndex = 0;
    boundTextures.clear();

    drawCalls=0;

}
void Renderer2D::draw(const Texture2D &texture2D, const Vec2f &pos, const Vec2f &size,
                      const Vec2f &origin, const Vec2f &scale,
                      const Color &color,
                      float rotation, bool flipX, bool flipY)
{

    if (drawOffset >= verticesSize)
        flush();

    // checking if texture already bound by any previous sprite
    if (boundTextures.find(texture2D.ID) == boundTextures.end())
    {
        // checking if there is enough space to bind texture (texture slots)
        if (boundTextures.size() > maxTextures)
            flush();

        boundTextures[texture2D.ID] = textureIndex;
        textureIndex++;
    }

    float packedColor = color.pack();
    float textureID = boundTextures[texture2D.ID];

    // using Model matrix would be overkill for this since its just for 2D

    float width = size.x;
    float height = size.y;

    if (scale.x != 1.0f)
        width *= scale.x;
    if (scale.y != 1.0f)
        height *= size.y * scale.y;

    float x1 = pos.x;
    float y1 = pos.y;
    float x2 = pos.x;
    float y2 = pos.y + height;
    float x3 = pos.x + width;
    float y3 = pos.y + height;
    float x4 = pos.x + width;
    float y4 = pos.y;

    // calculating rotation

    // bottom left
    vertices[drawOffset + 0] = flipX ? x4 : x1;
    vertices[drawOffset + 1] = flipY ? y2 : y1;
    vertices[drawOffset + 2] = 0.0f;
    vertices[drawOffset + 3] = 0.0f;
    vertices[drawOffset + 4] = packedColor;
    vertices[drawOffset + 5] = textureID; // texture

    // top left
    vertices[drawOffset + 6] = flipX ? x3 : x2;
    vertices[drawOffset + 7] = flipY ? y1 : y2;
    vertices[drawOffset + 8] = 0.0f;
    vertices[drawOffset + 9] = 1.0f;
    vertices[drawOffset + 10] = packedColor;
    vertices[drawOffset + 11] = textureID; // texture

    // top right
    vertices[drawOffset + 12] = flipX ? x2 : x3;
    vertices[drawOffset + 13] = flipY ? y4 : y3;
    vertices[drawOffset + 14] = 1.0f;
    vertices[drawOffset + 15] = 1.0f;
    vertices[drawOffset + 16] = packedColor;
    vertices[drawOffset + 17] = textureID; // texture

    // bottom right
    vertices[drawOffset + 18] = flipX ? x1 : x4;
    vertices[drawOffset + 19] = flipY ? y3 : y4;
    vertices[drawOffset + 20] = 1.0f;
    vertices[drawOffset + 21] = 0.0f;
    vertices[drawOffset + 22] = packedColor;
    vertices[drawOffset + 23] = textureID; // texture

    drawOffset += 24;
    drawElements += 6;
}

void Renderer2D::drawUnbatched(const Texture2D &texture2D, const Vec2f &pos, const Vec2f &size, const Vec2f &origin,
                               const Vec2f &scale, const Color &color, float rotation, bool flipX, bool flipY)
{
    if (drawing)
        throw std::runtime_error("Renderer2D::drawUnbatched: Cannot draw unbatched while batch is active");

    float packedColor = color.pack();

    // using Model matrix would be overkill for this since its just for 2D
    float width = size.x * scale.x;
    float height = size.y * scale.y;

    float x1 = pos.x;
    float y1 = pos.y;
    float x2 = pos.x;
    float y2 = pos.y + height;
    float x3 = pos.x + width;
    float y3 = pos.y + height;
    float x4 = pos.x + width;
    float y4 = pos.y;

    // bottom left
    vertices[0] = flipX ? x4 : x1;
    vertices[1] = flipY ? y2 : y1;
    vertices[2] = 0.0f;
    vertices[3] = 0.0f;
    vertices[4] = packedColor;
    vertices[5] = 0; // texture

    // top left
    vertices[6] = flipX ? x3 : x2;
    vertices[7] = flipY ? y1 : y2;
    vertices[8] = 0.0f;
    vertices[9] = 1.0f;
    vertices[10] = packedColor;
    vertices[11] = 0; // texture

    // top right
    vertices[12] = flipX ? x2 : x3;
    vertices[13] = flipY ? y4 : y3;
    vertices[14] = 1.0f;
    vertices[15] = 1.0f;
    vertices[16] = packedColor;
    vertices[17] = 0; // texture

    // bottom right
    vertices[18] = flipX ? x1 : x4;
    vertices[19] = flipY ? y3 : y4;
    vertices[20] = 1.0f;
    vertices[21] = 0.0f;
    vertices[22] = packedColor;
    vertices[23] = 0; // texture

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 24 * sizeof(float), vertices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    shader.bind();
    shader.setUniform("projectionMatrix", projMatrix);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);

}

void Renderer2D::flush()
{

    glBufferSubData(GL_ARRAY_BUFFER, 0, drawOffset * sizeof(float), vertices);
    shader.bind();
    shader.setUniform("projectionMatrix", projMatrix);

    // setting up textures
    int sampleCount = 0;
    for (auto &it:boundTextures)
    {
        //std::cout<<it.first<<"\t"<<it.second<<std::endl;
        glActiveTexture(GL_TEXTURE0 + it.second); // texture unit
        glBindTexture(GL_TEXTURE_2D, it.first); // texture id
        sampledTextures[sampleCount++] = it.second;
    }
    //shader.setUniform("textures",vals);
    //exit(1);
    shader.setUniform("textures",sampleCount,sampledTextures);


    glDrawElements(GL_TRIANGLES, drawElements, GL_UNSIGNED_INT, (void *) 0);
    drawCalls++;

    drawOffset = 0;
    drawElements = 0;

    textureIndex = 0;
    boundTextures.clear();
}
void Renderer2D::end()
{
    if (!drawing)
        throw std::runtime_error("Renderer2D::end(): Batch must first be started");

    flush();
    drawing = false;
}

void Renderer2D::setProjectionMatrix(Mat4f mat)
{
    projMatrix = mat;
}

unsigned int Renderer2D::getDrawCalls() const
{
    return drawCalls;
}
bool Renderer2D::isDrawing() const
{
    return drawing;
}
unsigned int Renderer2D::getVertexSize() const
{
    return vertexSize;
}

unsigned int Renderer2D::getVerticesSize() const
{
    return verticesSize;
}

unsigned int Renderer2D::getIndicesSize() const
{
    return indicesSize;
}


