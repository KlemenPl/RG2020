//
// Created by klemen on 06/12/2020.
//

#ifndef TOWERDEFENSE_RENDERER2D_H
#define TOWERDEFENSE_RENDERER2D_H

#include "../Core.h"
#include "../graphics/Shader.h"
#include "../graphics/Texture2D.h"
#include "../graphics/Color.h"
#include "../graphics/BitmapFont.h"

/*
 * TODO 12.12.2020: generate the corner points in a geometry shader with a single center point, rotation and size
 * (will help to reduce load on the CPU)
 *
 * TODO 12.12.2020: use "Instancing" approach to see, if performance is better
 *
 */

/*
 * Renderer for rendering 2D stuff (sprites, text, primitives...)
 * Uses batching for better performance.
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
    Ref<Shader> shader;
    uint32_t VAO{};
    uint32_t VBO{};
    uint32_t IBO{};
    uint32_t quadsPerBatch{};

    const glm::mat4 *projMatrix{};

    uint32_t verticesSize{};
    uint32_t indicesSize{};
    float *vertices{};
    uint32_t *indices{};

    uint32_t drawCalls{};
    uint32_t drawOffset{};
    uint32_t drawElements{};
    bool drawing{};

    uint32_t maxTextures{};
    uint32_t textureIndex{};

    //__gnu_cxx::hash_map<uint32_t, uint32_t> boundTextures; slightly faster
    std::unordered_map<uint32_t, uint32_t> boundTextures;
    int *sampledTextures{};

public:
    explicit Renderer2D(Ref<Shader> shader);

    ~Renderer2D();

    void draw(const UVRegion &region, const glm::vec2 &pos, const glm::vec2 &size,
              float originX = 0, float originY = 0,
              float scaleX = 1.0f, float scaleY = 1.0f,
              const Color &color = Colors::WHITE,
              float rotation = 0.0f, bool flipX = false, bool flipY = false);

    void draw(const BitmapFont &font, const std::string &text, const glm::vec2 &pos,
              float scaleX = 1.0f, float scaleY = 1.0f,
              const Color &color = Colors::WHITE, float rotation = 0.0f);

    void begin();

    void flush();

    void end();

    void setProjectionMatrix(const glm::mat4 &mat);

    uint32_t getDrawCalls() const;
    bool isDrawing() const;
    uint32_t getVerticesSize() const;
    uint32_t getIndicesSize() const;

    // should not be copied!!
    Renderer2D(const Renderer2D &) = delete;
    Renderer2D() = default;
    Renderer2D &operator=(const Renderer2D &) = delete;


};


#endif //TOWERDEFENSE_RENDERER2D_H
