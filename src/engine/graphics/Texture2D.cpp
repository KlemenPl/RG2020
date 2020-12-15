//
// Created by klemen on 07/12/2020.
//

#include "Texture2D.h"

Texture2D::Texture2D() : width(0), height(0), internalFormat(GL_RGB), imageFormat(GL_RGB), wrapS(GL_REPEAT),
                         wrapT(GL_REPEAT), filterMin(GL_LINEAR), filterMag(GL_LINEAR)
{
}

Texture2D::~Texture2D()
{
    // clean up
    glDeleteTextures(1, &this->ID);
}
void Texture2D::generate(uint32_t _width, uint32_t _height, unsigned char *data)
{
    glGenTextures(1, &this->ID);
    this->width = _width;
    this->height = _height;
    // create Texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, _width, _height, 0, this->imageFormat, GL_UNSIGNED_BYTE, data);
    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMag);
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

}
void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}

float Texture2D::getU0() const
{
    return 0.0f;
}
float Texture2D::getV0() const
{
    return 0.0f;
}
float Texture2D::getU1() const
{
    return 1.0f;
}
float Texture2D::getV1() const
{
    return 1.0f;
}

uint32_t Texture2D::getID()const
{
    return ID;
}
