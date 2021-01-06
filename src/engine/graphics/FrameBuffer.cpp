//
// Created by klemen on 25/12/2020.
//

#include "FrameBuffer.h"
#include "../../game/Game.h"

FrameBuffer::FrameBuffer(uint32_t _width, uint32_t _height)
        : width(_width), height(_height)
//: width(1280), height(720)
{
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
    delete colourAttachment;
    delete depthAttachment;
    delete stencilAttachment;

    for (auto i:renderBuffers)
        glDeleteRenderbuffers(1, &i);

    glDeleteBuffers(1, &fboID);
}
void FrameBuffer::bind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glViewport(0, 0, width, height);

    fbStack->push(this);
}
void FrameBuffer::unbind() const
{
    fbStack->pop();
    const FrameBuffer *buffer = nullptr;
    if (!fbStack->empty())
    {
        buffer = fbStack->top();
        glViewport(0, 0, buffer->width, buffer->height);
    }
    else
        glViewport(0, 0, Game::width, Game::height);

    glBindFramebuffer(GL_FRAMEBUFFER, buffer == nullptr ? 0 : buffer->fboID);
}

void FrameBuffer::createColourAttachment(GLint format,
                                         uint32_t wrapS, uint32_t wrapT,
                                         uint32_t filterMin, uint32_t filterMag)
{
    delete colourAttachment;

    bind();
    colourAttachment = new Texture2D();
    colourAttachment->width = width;
    colourAttachment->height = height;
    colourAttachment->wrapS = wrapS;
    colourAttachment->wrapT = wrapT;
    colourAttachment->filterMin = filterMin;
    colourAttachment->filterMag = filterMag;

    glGenTextures(1, &colourAttachment->ID);
    glBindTexture(GL_TEXTURE_2D, colourAttachment->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, (void *) 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colourAttachment->ID, 0);

    uint32_t depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    renderBuffers.push_back(depthBuffer);

    unbind();
}

void FrameBuffer::createDepthAndColourAttachment(uint32_t filterMin, uint32_t filterMag)
{
    delete colourAttachment;
    delete depthAttachment;

    bind();
    colourAttachment = new Texture2D();
    colourAttachment->width = width;
    colourAttachment->height = height;
    colourAttachment->filterMin = filterMin;
    colourAttachment->filterMag = filterMag;

    depthAttachment = new Texture2D();
    depthAttachment->width = width;
    depthAttachment->height = height;
    depthAttachment->filterMin = filterMin;
    depthAttachment->filterMag = filterMag;

    glGenTextures(1, &colourAttachment->ID);
    glBindTexture(GL_TEXTURE_2D, colourAttachment->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void *) 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colourAttachment->ID, 0);

    glGenTextures(1, &depthAttachment->ID);
    glBindTexture(GL_TEXTURE_2D, depthAttachment->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 (void *) 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAttachment->ID, 0);
    unbind();
}

void FrameBuffer::createDepthAttachment(uint32_t filterMin, uint32_t filterMag)
{
    depthAttachment = new Texture2D();
    depthAttachment->width = width;
    depthAttachment->height = height;
    depthAttachment->filterMin = filterMin;
    depthAttachment->filterMag = filterMag;

    glGenTextures(1, &depthAttachment->ID);
    glBindTexture(GL_TEXTURE_2D, depthAttachment->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fboID, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
