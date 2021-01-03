//
// Created by klemen on 26/12/2020.
//

#ifndef TOWERDEFENSE_TERRAIN_H
#define TOWERDEFENSE_TERRAIN_H

#include "Mesh.h"
#include "Color.h"
#include "Model.h"
#include "Texture2D.h"

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

class WaterFrameBuffers
{
private:
    inline static const int REFLECTION_WIDTH = 320;
    inline static const int REFLECTION_HEIGHT = 180;

    inline static const int REFRACTION_WIDTH = 1280;
    inline static const int REFRACTION_HEIGHT = 720;

    uint32_t reflectionFrameBuffer;
    uint32_t reflectionTexture;
    uint32_t reflectionDepthBuffer;

    uint32_t refractionFrameBuffer;
    uint32_t refractionTexture;
    uint32_t refractionDepthTexture;


public:
    WaterFrameBuffers()
    {//call when loading the game
        initialiseReflectionFrameBuffer();
        initialiseRefractionFrameBuffer();
    }

    void cleanUp()
    {
        glDeleteFramebuffers(1, &reflectionFrameBuffer);
        glDeleteTextures(1, &reflectionTexture);
        glDeleteRenderbuffers(1, &reflectionDepthBuffer);
        glDeleteFramebuffers(1, &refractionFrameBuffer);
        glDeleteTextures(1, &refractionTexture);
        glDeleteTextures(1, &refractionDepthTexture);
    }

    void bindReflectionFrameBuffer() const
    {
        //call before rendering to this FBO
        bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
    }

    void bindRefractionFrameBuffer()  const
    {
        //call before rendering to this FBO
        bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
    }

    void unbindCurrentFrameBuffer()  const
    {
        //call to switch to default frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 1280, 720);
    }

    uint32_t getReflectionTexture()  const
    {
        //get the resulting texture
        return reflectionTexture;
    }
    uint32_t getRefractionTexture() const
    {
        //get the resulting texture
        return refractionTexture;
    }

    uint32_t getRefractionDepthTexture() const
    {
        //get the resulting depth texture
        return refractionDepthTexture;
    }

private:
    void initialiseReflectionFrameBuffer()
    {
        reflectionFrameBuffer = createFrameBuffer();
        reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
        reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
        unbindCurrentFrameBuffer();
    }

    void initialiseRefractionFrameBuffer()
    {
        refractionFrameBuffer = createFrameBuffer();
        refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
        refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
        unbindCurrentFrameBuffer();
    }

    void bindFrameBuffer(uint32_t frameBuffer, int width, int height)  const
    {
        glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glViewport(0, 0, width, height);
    }

    uint32_t createFrameBuffer()  const
    {
        uint32_t frameBuffer;
        glGenFramebuffers(1, &frameBuffer);
        //generate name for frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        //create the framebuffer
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        //indicate that we will always render to color attachment 0
        return frameBuffer;
    }

    uint32_t createTextureAttachment(int width, int height)  const
    {
        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void *) 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
        return texture;
    }

    uint32_t createDepthTextureAttachment(int width, int height)  const
    {
        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                     (void *) 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
        return texture;
    }

    uint32_t createDepthBufferAttachment(int width, int height) const
    {
        uint32_t depthBuffer;
        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
        return depthBuffer;
    }
};

class Terrain
{
private:
    Model *terrainModels = nullptr;
    uint32_t terrianModelsLength = 0;
public:
    Mesh waterMesh;
    Mesh terrainMesh;

    WaterFrameBuffers waterFrameBuffers;

    float **heights = nullptr;
    uint32_t heightsWidth;
    uint32_t heightsHeight; // ¯\_(ツ)_/¯

    float minHeight;
    float maxHeight;

    float waterLevel = 0;

    Ref<Texture2D> waterDuDvMap;
    Ref<Texture2D> waterNormalMap;

    Terrain();

    void generate(uint32_t xSize, uint32_t ySize, uint32_t detailX, uint32_t detailY, uint32_t seed = 20,
                  uint32_t resolution = 1.0f, const Biome &biome = {});

    float getHeight(float xPos, float yPos);
    virtual ~Terrain();

};

#endif //TOWERDEFENSE_TERRAIN_H
