//
// Created by klemen on 25/12/2020.
//

#ifndef TOWERDEFENSE_FRAMEBUFFER_H
#define TOWERDEFENSE_FRAMEBUFFER_H

#include "../Core.h"
#include "Texture2D.h"
#include <vector>
#include <stack>


/*
 * Very simple utility class for
 * dealing with framebuffers.
 */
class FrameBuffer
{
private:
    inline static std::stack<const FrameBuffer *> *fbStack = new std::stack<const FrameBuffer *>();
    std::vector<uint32_t> renderBuffers;

    void validate();
public:
    const uint32_t width;
    const uint32_t height;

    Texture2D *colourAttachment = nullptr;
    Texture2D *depthAttachment = nullptr;
    Texture2D *stencilAttachment = nullptr;

    uint32_t fboID{};

    FrameBuffer(uint32_t _width, uint32_t _height);
    ~FrameBuffer();

    void bind() const;
    void unbind() const;

    void dispose();

    void createColourAttachment(GLint format = GL_RGB,
                                uint32_t wrapS = GL_REPEAT,
                                uint32_t wrapT = GL_REPEAT,
                                uint32_t filterMin = GL_LINEAR,
                                uint32_t filterMag = GL_LINEAR);
    void createDepthAndColourAttachment(uint32_t filterMin = GL_LINEAR,
                                        uint32_t filterMag = GL_LINEAR);

    void createDepthAttachment(uint32_t filterMin = GL_LINEAR,
                               uint32_t filterMag = GL_LINEAR);

    // should not be copied!!
    FrameBuffer(const FrameBuffer &) = delete;
    //FrameBuffer() = default;
    FrameBuffer &operator=(const FrameBuffer &) = delete;
};

#endif //TOWERDEFENSE_FRAMEBUFFER_H
