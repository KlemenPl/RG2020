//
// Created by klemen on 06/12/2020.
//

#include <iostream>
#include <cmath>
#include "Renderer2D.h"
#include "RenderingCapabilities.h"
#include "../graphics/Vertex2D.h"
#include "../ResourceManager.h"


/*
 * Constructor for Renderer2D.
 * Takes in a pointer to the Shader for drawing.
 */
Renderer2D::Renderer2D(Ref<Shader> shader) : shader(shader)
{
    this->maxTextures = RenderingCapabilities::MAX_TEXTURE_IMAGE_UNITS;
    //this->maxTextures = 20;
    this->sampledTextures = new int[maxTextures];

    for (int i = 0; i < maxTextures; i++)
        sampledTextures[i] = 0;

    this->drawing = false;
    this->drawCalls = 0;
    this->textureIndex = 0;

    this->quadsPerBatch = 5000; // 5k sprites per batch seems to work the best
    this->indicesSize = quadsPerBatch * 6; // 6 indices per quad
    this->verticesSize = quadsPerBatch * 4 * 6; // 4 points per quad; each point with 6 floats

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
    //glBufferStorage(GL_ARRAY_BUFFER, verticesSize * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, pos));

    glEnableVertexAttribArray(1); // texture position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, texCoords));

    glEnableVertexAttribArray(2); // colour
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, color));

    /* Would only need 5 bits to store this 32 values, but I think it's faster,
     * if you keep it as 4 byte float (alignment of vertices -
     * https://stackoverflow.com/questions/12819188/how-important-is-alignment-in-opengl-vertex-data-in-ios).
     * And this is a lot simpler and faster to implement.
     */
    glEnableVertexAttribArray(3); // texture ID
    glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(Vertex2D), (const void *) offsetof(Vertex2D, textureID));

}

/*
 * Destructor
 *
 * Deletes the VBO and heap allocated memory
 */
Renderer2D::~Renderer2D()
{
    glDeleteBuffers(GL_ARRAY_BUFFER, &VBO);
    glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &IBO);
    glDeleteVertexArrays(1, &VAO);

    delete[] indices;
    delete[] vertices;
    delete[] sampledTextures;
}


/*
 * Begins the batch.
 */
void Renderer2D::begin()
{
    if (drawing)
        throw std::runtime_error("Renderer2D::begin(): Cannot begin batch while it is already drawing!");

    drawing = true;
    drawOffset = 0;
    drawElements = 0;
    textureIndex = 0;
    boundTextures.clear();

    drawCalls = 0;

    // Disabling culling since it is not really needed for 2D
    // and does not improve performance for 2D that much.
    // It also does not render if flipX or flipY are set to true.
    glDisable(GL_CULL_FACE);

    // binding VBA, VBO, IBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // enable transperency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader->bind();
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
void Renderer2D::draw(const UVRegion &region, const glm::vec2 &pos, const glm::vec2 &size,
                      float originX, float originY,
                      float scaleX, float scaleY,
                      const Color &color,
                      float rotation, bool flipX, bool flipY)
{

    // flushing if batch is full
    if (drawOffset >= verticesSize)
        flush();

    uint32_t textureID = region.getID();


    // todo 13.12.2020: (Renderer2D optimization) find a better way, this takes around 37% of the whole draw call
    const auto &t_texture = boundTextures.find(textureID);

    float samplerIndex;

    // checking if texture already bound by any previous sprite
    if (t_texture == boundTextures.end())
    {
        // checking if there is enough space to bind the texture (texture slots)
        if (boundTextures.size() >= maxTextures)
            flush();

        // binding texture
        boundTextures[textureID] = textureIndex;
        samplerIndex = textureIndex;
        textureIndex++;
    } else
    {
        samplerIndex = t_texture->second;
    }


    // takes 4 float components from Color.rgba and packs them into a float
    // todo 13.12.2020: (Renderer2D optimization) only set colour when changed
    float packedColor = color.pack(); // this takes 5% of the whole draw call

    // using Model matrix would be a lot simpler, but this does the same thing faster in my opinion
    float spaceOriginX = pos.x + originX;
    float spaceOriginY = pos.y + originY;

    // to avoid calculating again when assigning to corners
    float deltaOriginX1 = -originX;
    float deltaOriginX2 = size.x;

    float deltaOriginY1 = -originX;
    float deltaOriginY2 = size.y - originX;

    // applying scale if needed
    if (scaleX != 1.0f)
    {
        deltaOriginX1 *= scaleX;
        deltaOriginX2 *= scaleX;
    }
    if (scaleY != 1.0f)
    {
        deltaOriginY1 *= scaleY;
        deltaOriginY2 *= scaleY;
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
    if (rotation != 0)
    {
        // cos -sin 0
        // sin cos  0
        // 0   0    1


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

    drawOffset += 24; // IBO offset
    drawElements += 6; // IBO offset
}

// utility function for drawing fonts
float abs(float in)
{
    return in < 0 ? -in : in;
}

/*
 * Draws text on the screen at specified position.
 * Will use Renderer2D::draw(Region&...) for drawing the actual glyhs
 */
void Renderer2D::draw(const BitmapFont &font, const std::string &text, const glm::vec2 &pos,
                      float scaleX, float scaleY,
                      const Color &color, float rotation)
{
    float advance = pos.x;

    for (const char &c:text)
    {
        const FontCharacter fc = font.charData.at(c);
        float sizeX = (abs(fc.xOff) + abs(fc.xOff2)) * scaleX;
        float sizeY = (abs(fc.yOff) + abs(fc.yOff2)) * scaleY;

        // drawing the character
        Renderer2D::draw(fc.region,
                         glm::vec2(advance + fc.xOff * scaleX, pos.y - fc.yOff * scaleY),
                         glm::vec2(sizeX, -sizeY),
                         0.0f, 0.0f, // origin
                         1.0f, 1.0f, // scale
                         color, rotation, false, false);

        // moving to the next position
        advance += fc.xAdvance * scaleX;
    }

}

/*
 * Flushes the batch and resets it.
 *
 * First it sends the data from client to GPU. Then it
 * binds all used textures and calls glDrawElements(...) to
 * draw the mesh.
 */
void Renderer2D::flush()
{

    // updating VBO (sending only the verticies we assigned)
    glBufferSubData(GL_ARRAY_BUFFER, 0, drawOffset * sizeof(float), vertices);

    // todo: only set projMatrix when projMatrix is changed
    shader->setUniform("projectionMatrix", *projMatrix);

    // binding textures
    for (auto &it:boundTextures)
    {
        glActiveTexture(GL_TEXTURE0 + it.second); // texture unit
        glBindTexture(GL_TEXTURE_2D, it.first); // texture id
        sampledTextures[it.second] = it.second; // sampler2D index
        //std::cout<<"first: "<<it.first<<"\t second: "<<it.second<<std::endl;
    }

    // setting sampler2D[] uniform
    shader->setUniform("textures", maxTextures, sampledTextures);

    // drawing elements
    glDrawElements(GL_TRIANGLES, drawElements, GL_UNSIGNED_INT, nullptr);
    drawCalls++;

    // unbinding textures, reseting state
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

/*
 * Flushes and ends the batch
 */
void Renderer2D::end()
{
    if (!drawing)
        throw std::runtime_error("Renderer2D::end(): Batch must first be started");

    flush();
    drawing = false;
}

/*
 * Sets the projection matrix to be used in shader
 * when drawing.
 */
void Renderer2D::setProjectionMatrix(const glm::mat4 &mat)
{
    projMatrix = &mat;
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
