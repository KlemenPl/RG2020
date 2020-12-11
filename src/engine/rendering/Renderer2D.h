//
// Created by klemen on 06/12/2020.
//

#ifndef TOWERDEFENSE_RENDERER2D_H
#define TOWERDEFENSE_RENDERER2D_H

#include <glad/glad.h>
#include "../graphics/Shader.h"
#include "../graphics/Texture2D.h"
#include "../graphics/Color.h"
#include "../math/Matrix.h"
#include "../math/Vector.h"

struct Character {
    unsigned int textureID;
    Vec2i size;
    Vec2i bearing;
    unsigned int advance;
};

/*
 * Renderer for rendering 2D stuff (sprites, text, primitives...)
 * Uses batching for performance. (Handles 100k sprites without a problem,
 * when drawing without batching, it can bearly handle 10k sprites)
 *
 * One vertex has 24 bytes. To render a quad you need 96 bytes.
 *
 * Instead of drawing each quad individually, it saves its vertex data into vertices array on the CPU. When the
 * batch is full, it will be flushed. All data from CPU array is copied to VBO at once and then drawn to screen with
 * only 1 draw call.
 */
class Renderer2D
{

private:
    Shader shader;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int IBO;
    unsigned int quadsPerBatch;
    Mat4f projMatrix{};

    unsigned int verticesSize;
    unsigned int indicesSize;
    float *vertices;
    unsigned int *indices;

    unsigned int drawCalls;
    unsigned int drawOffset;
    unsigned int drawElements;
    unsigned int vertexSize;
    bool drawing;

    unsigned int maxTextures;
    unsigned int textureIndex;

    std::unordered_map<unsigned int, unsigned int> boundTextures;
    int* sampledTextures;

    std::unordered_map<char, Character> characters;

public:
    explicit Renderer2D(Shader shader);

    ~Renderer2D();

    void draw(const Texture2D &texture2D, const Vec2f &pos, const Vec2f &size,
              const Vec2f &origin = Vec2f{0.0f, 0.0f}, const Vec2f &scale = Vec2f{1.0f, 1.0f},
              const Color &color = Color{1.0f, 1.0f, 1.0f, 1.0f},
              float rotation = 0.0f, bool flipX = false, bool flipY = false);

    void drawUnbatched(const Texture2D &texture2D, const Vec2f &pos, const Vec2f &size,
              const Vec2f &origin = Vec2f{0.0f, 0.0f}, const Vec2f &scale = Vec2f{1.0f, 1.0f},
              const Color &color = Color{1.0f, 1.0f, 1.0f, 1.0f},
              float rotation = 0.0f, bool flipX = false, bool flipY = false);

    void begin();

    void flush();

    void end();

    void setProjectionMatrix(Mat4f mat);

    // should not be copied!!
    Renderer2D(const Renderer2D &) = delete;
    Renderer2D() = default;
    Renderer2D &operator=(const Renderer2D &) = delete;
    unsigned int getDrawCalls() const;
    bool isDrawing() const;
    unsigned int getVertexSize() const;
    unsigned int getVerticesSize() const;
    unsigned int getIndicesSize() const;


};


#endif //TOWERDEFENSE_RENDERER2D_H
