//
// Created by klemen on 25/12/2020.
//

#ifndef TOWERDEFENSE_FRAMEBUFFER_H
#define TOWERDEFENSE_FRAMEBUFFER_H

#include <memory>
/*
 * Very simple utility class for
 * dealing with framebuffers.
 */
class FrameBuffer
{
public:
    uint32_t fbo{};

    FrameBuffer();
    ~FrameBuffer();

    void bind() const;
    void unbind() const;

    // should not be copied!!
    FrameBuffer(const FrameBuffer &) = delete;
    //FrameBuffer() = default;
    FrameBuffer &operator=(const FrameBuffer &) = delete;
};
#endif //TOWERDEFENSE_FRAMEBUFFER_H
