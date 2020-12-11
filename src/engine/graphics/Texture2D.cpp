//
// Created by klemen on 07/12/2020.
//

#include <iostream>
#include "Texture2D.h"

Texture2D::Texture2D() : width(0), height(0), internalFormat(GL_RGB), imageFormat(GL_RGB), wrapS(GL_REPEAT),
                         wrapT(GL_REPEAT), filterMin(GL_LINEAR), filterMax(GL_LINEAR)
{
}

void Texture2D::dispose()
{
    glDeleteTextures(1, &this->ID);
}
void Texture2D::generate(unsigned int width, unsigned int height, unsigned char *data)
{
    glGenTextures(1, &this->ID);
    this->width = width;
    this->height = height;
    // create Texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, width, height, 0, this->imageFormat, GL_UNSIGNED_BYTE, data);
    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMax);
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

}
void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}
