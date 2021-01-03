//
// Created by klemen on 25/12/2020.
//

#include "FrameBuffer.h"
#include "../Core.h"

FrameBuffer::FrameBuffer(uint32_t _width, uint32_t _height)
        : width(_width), height(_height)
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

    glDeleteBuffers(1, &fboID);
}
void FrameBuffer::bind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glViewport(0, 0, width, height);
}
void FrameBuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::createColourAttachment(GLint format,
                                         uint32_t wrapS, uint32_t wrapT,
                                         uint32_t filterMin, uint32_t filterMag)
{
    delete colourAttachment;

    colourAttachment = new Texture2D();
    colourAttachment->width = width;
    colourAttachment->height = width;
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
}

void FrameBuffer::createDepthAttachment(uint32_t filterMin, uint32_t filterMag)
{
    delete depthAttachment;

    depthAttachment = new Texture2D();
    depthAttachment->width = width;
    depthAttachment->height = height;
    depthAttachment->filterMin = filterMin;
    depthAttachment->filterMag = filterMag;

    glGenTextures(1, &depthAttachment->ID);
    glBindTexture(GL_TEXTURE_2D, depthAttachment->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 (void *) 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAttachment->ID, 0);
}
