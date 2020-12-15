//
// Created by klemen on 07/12/2020.
//

#ifndef TOWERDEFENSE_TEXTURE2D_H
#define TOWERDEFENSE_TEXTURE2D_H

#include <glad/glad.h>
#include "UVRegion.h"


class Texture2D : public UVRegion
{
public:

    uint32_t ID{};
    uint32_t width, height;

    // texture format
    uint32_t internalFormat;
    uint32_t imageFormat;

    // texture configuration
    uint32_t wrapS;
    uint32_t wrapT;
    uint32_t filterMin;
    uint32_t filterMag;

    Texture2D();

    void generate(uint32_t width, uint32_t height, unsigned char *data);

    void bind() const override;
    float getU0() const override;
    float getV0() const override;
    float getU1() const override;
    float getV1() const override;
    uint32_t getID()const override;
    virtual ~Texture2D();


};


#endif //TOWERDEFENSE_TEXTURE2D_H
