//
// Created by klemen on 06/12/2020.
//

#include <iostream>
#include <cmath>
#include "Renderer2D.h"
#include "RenderingCapabilities.h"
#include "../graphics/Vertex2D.h"
#include "../ResourceManager.h"


Renderer2D::Renderer2D(const Shader shader) : shader(shader)
{
    // configuring VAO/VBO/IBO
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
    //this->maxTextures = 20;
    this->sampledTextures = new int[maxTextures];

    for (int i = 0; i < maxTextures; i++)
        sampledTextures[i] = 0;

    this->drawing = false;
    this->drawCalls = 0;
    this->textureIndex = 0;

    this->quadsPerBatch = 5000; // 5k sprites per batch seems to work the best
    this->indicesSize = quadsPerBatch * 6;
    this->verticesSize = quadsPerBatch * 4 * 6;

    this->vertices = new float[verticesSize];
    for (int i = 0; i < verticesSize; i++)
        vertices[i] = 0;

    this->indices = new uint32_t[indicesSize];
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(uint32_t), indices, GL_STATIC_DRAW);


    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, pos));

    glEnableVertexAttribArray(1); // texture position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, texCoords));

    glEnableVertexAttribArray(2); // colour
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, color));

    glEnableVertexAttribArray(3); // texture ID
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_TRUE, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, textureID));

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

    drawCalls = 0;

}
/*
 * Renders 2D texture at the specified position.
 * Uses a batch for better performance. Instead of
 * drawing each quad with its own openGL call, it will
 * copy the vertex data into the vertices array (heap memory).
 *
 * Once the batch is full or the Renderer2d::end() is called, it
 * will send data to the GPU and draw it with only one render call.
 *
 * Only supports rotating on Z-axis (assumes orthographic projection), X and Y
 * axis can be simulated by chaing size or scale.
 *
 */
void Renderer2D::draw(const UVRegion &region, const Vec2f &pos, const Vec2f &size,
                      const Vec2f &origin, const Vec2f &scale,
                      const Color &color,
                      float rotation, bool flipX, bool flipY)
{

    // flushing if batch is full
    if (drawOffset >= verticesSize)
        flush();

    uint32_t textureID = region.getID();

    // checking if texture already bound by any previous sprite
    if (boundTextures.find(textureID) == boundTextures.end())
    {
        // checking if there is enough space to bind the texture (texture slots)
        if (boundTextures.size() >= maxTextures)
            flush();

        // binding texture
        boundTextures[textureID] = textureIndex;
        textureIndex++;
    }


    /* Would only need 5 bits to store this 32 values, but I think it's faster,
     * if you keep it as 4 byte float (alignment of vertices -
     * https://stackoverflow.com/questions/12819188/how-important-is-alignment-in-opengl-vertex-data-in-ios).
     * And this is a lot simpler and faster to implement.
     */
    float samplerIndex = boundTextures[textureID];


    // takes 4 float components from Color.rgba and packs them into a float
    float packedColor = color.pack();

    // using Model matrix would be a lot simpler, but this does the same faster
    float spaceOriginX = pos.x + origin.x;
    float spaceOriginY = pos.y + origin.y;

    // to avoid calculating again when assigning to corners
    float deltaOriginX1 = -origin.x;
    float deltaOriginX2 = size.x;

    float deltaOriginY1 = -origin.x;
    float deltaOriginY2 = size.y - origin.x;

    // applying scale if needed
    if (scale.x != 1.0f)
    {
        deltaOriginX1 *= scale.x;
        deltaOriginX2 *= scale.x;
    }
    if (scale.y != 1.0f)
    {
        deltaOriginY1 *= scale.y;
        deltaOriginY2 *= scale.y;
    }


    // bottom left
    float x1 = deltaOriginX1;
    float y1 = deltaOriginY1;

    // top left
    float x2 = deltaOriginX1;
    float y2 = deltaOriginY2;

    // top right
    float x3 = deltaOriginX2;
    float y3 = deltaOriginY2;

    // botom right
    float x4 = deltaOriginX2;
    float y4 = deltaOriginY1;

    //rotation = math::toRad(-45.0f);
    // calculating rotation
    if (rotation != 0)
    {
        // based on:
        // https://stackoverflow.com/questions/2259476/rotating-a-point-about-another-point-2d

        // applying rotation
        const float sin = sinf(rotation);
        const float cos = cosf(rotation);

        float t_x1 = cos * x1 - sin * y1;
        float t_y1 = sin * x1 + cos * y1;

        float t_x2 = cos * x2 - sin * y2;
        float t_y2 = sin * x2 + cos * y2;

        float t_x3 = cos * x3 - sin * y3;
        float t_y3 = sin * x3 + cos * y3;

        float t_x4 = cos * x4 - sin * y4;
        float t_y4 = sin * x4 + cos * y4;

        x1 = t_x1 + spaceOriginX;
        y1 = t_y1 + spaceOriginY;

        x2 = t_x2 + spaceOriginX;
        y2 = t_y2 + spaceOriginY;

        x3 = t_x3 + spaceOriginX;
        y3 = t_y3 + spaceOriginY;

        x4 = t_x4 + spaceOriginX;
        y4 = t_y4 + spaceOriginY;

    } else
    {
        x1 += pos.x;
        y1 += pos.y;

        x2 += pos.x;
        y2 += pos.y;

        x3 += pos.x;
        y3 += pos.y;

        x4 += pos.x;
        y4 += pos.y;
    }

    //std::cout << "id: " << samplerIndex << "\t texID: " << textureID << std::endl;

    // flipping if needed
    if (flipX)
    {
        float tmp = x4;
        x4 = x1;
        x1 = tmp;

        tmp = x3;
        x3 = x2;
        x2 = tmp;
    }
    if (flipY)
    {
        float tmp = y2;
        y2 = y1;
        y1 = tmp;

        tmp = y4;
        y4 = y3;
        y3 = tmp;
    }

    // bottom left
    vertices[drawOffset + 0] = x1;
    vertices[drawOffset + 1] = y1;
    vertices[drawOffset + 2] = region.getU0();
    vertices[drawOffset + 3] = region.getV0();
    vertices[drawOffset + 4] = packedColor;
    vertices[drawOffset + 5] = samplerIndex; // texture

    // top left
    vertices[drawOffset + 6] = x2;
    vertices[drawOffset + 7] = y2;
    vertices[drawOffset + 8] = region.getU0();
    vertices[drawOffset + 9] = region.getV1();
    vertices[drawOffset + 10] = packedColor;
    vertices[drawOffset + 11] = samplerIndex; // texture

    // top right
    vertices[drawOffset + 12] = x3;
    vertices[drawOffset + 13] = y3;
    vertices[drawOffset + 14] = region.getU1();
    vertices[drawOffset + 15] = region.getV1();
    vertices[drawOffset + 16] = packedColor;
    vertices[drawOffset + 17] = samplerIndex; // texture

    // bottom right
    vertices[drawOffset + 18] = x4;
    vertices[drawOffset + 19] = y4;
    vertices[drawOffset + 20] = region.getU1();
    vertices[drawOffset + 21] = region.getV0();
    vertices[drawOffset + 22] = packedColor;
    vertices[drawOffset + 23] = samplerIndex; // texture

    drawOffset += 24; // vertices offset
    drawElements += 6; // IBO offset
}

// utility function for drawing fonts
float abs(float in)
{
    return in < 0 ? -in : in;
}

void Renderer2D::draw(BitmapFont &font, const std::string &text, const Vec2f &pos,
                      const Vec2f &scale, const Color &color, float rotation)
{
    float advance = pos.x;
    for (const char &c:text)
    {
        const FontCharacter fc = font.charData.at(c);
        float sizeX = (abs(fc.xOff) + abs(fc.xOff2)) * scale.x;
        float sizeY = (abs(fc.yOff) + abs(fc.yOff2))*scale.y;

        Renderer2D::draw(fc.region,
                         Vec2f(advance + fc.xOff*scale.x, pos.y - fc.yOff*scale.y),
                         Vec2f(sizeX, -sizeY), Vec2f(0, 0), Vec2f(1.0f, 1.0f),
                         color, rotation, false, false);

        advance += fc.xAdvance*scale.x;
    }

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
    // binding textures
    //std::cout << "Texture index: " << textureIndex << std::endl;

    for (auto &it:boundTextures)
    {
        glActiveTexture(GL_TEXTURE0 + it.second); // texture unit
        glBindTexture(GL_TEXTURE_2D, it.first); // texture id
        sampledTextures[it.second] = it.second;
        //std::cout<<"first: "<<it.first<<"\t second: "<<it.second<<std::endl;
    }

    shader.setUniform("textures", maxTextures, sampledTextures);

    // drawing elements
    glDrawElements(GL_TRIANGLES, drawElements, GL_UNSIGNED_INT, (void *) 0);
    drawCalls++;

    // unbinding textures
    for (int i = 0; i < textureIndex; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // texture unit
        glBindTexture(GL_TEXTURE_2D, 0); // texture id
        sampledTextures[i] = 0;
    }


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

uint32_t Renderer2D::getDrawCalls() const
{
    return drawCalls;
}
bool Renderer2D::isDrawing() const
{
    return drawing;
}

uint32_t Renderer2D::getVerticesSize() const
{
    return verticesSize;
}

uint32_t Renderer2D::getIndicesSize() const
{
    return indicesSize;
}
