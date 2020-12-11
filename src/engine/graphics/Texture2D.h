//
// Created by klemen on 07/12/2020.
//

#ifndef TOWERDEFENSE_TEXTURE2D_H
#define TOWERDEFENSE_TEXTURE2D_H

#include <glad/glad.h>

class Texture2D
{
public:
    unsigned int ID{};
    unsigned int width, height;

    // texture format
    unsigned int internalFormat;
    unsigned int imageFormat;

    // texture configuration
    unsigned int wrapS;
    unsigned int wrapT;
    unsigned int filterMin;
    unsigned int filterMax;

    Texture2D();

    void generate(unsigned int width, unsigned int height, unsigned char* data);

    void dispose();

    void bind() const;

};


#endif //TOWERDEFENSE_TEXTURE2D_H
