//
// Created by klemen on 25/12/2020.
//

#include "FrameBuffer.h"
#include "../Core.h"

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &fbo);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteBuffers(1, &fbo);
}
void FrameBuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
void FrameBuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
