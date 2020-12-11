//
// Created by klemen on 09/12/2020.
//

#ifndef TOWERDEFENSE_RENDERINGCAPABILITIES_H
#define TOWERDEFENSE_RENDERINGCAPABILITIES_H

#include <glad/glad.h>

class RenderingCapabilities
{
private:
    inline static bool initiated = false;
public:
    inline static int MAX_VERTEX_ATTRIBS;
    inline static int MAX_COMBINED_TEXTURE_IMAGE_UNITS;
    inline static int MAX_TEXTURE_IMAGE_UNITS; // max on fragment shader
    inline static int MAX_VERTEX_TEXTURE_IMAGE_UNITS;
    inline static int MAX_GEOMETRY_TEXTURE_IMAGE_UNITS;

    inline static int MAX_TEXTURE_SIZE;

    static void init()
    {
        if (initiated)
        {
            std::cout << "[Warning] RenderingCapabilities::init(): Already initiated." << std::endl;
            return;
        }

        initiated = true;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &MAX_VERTEX_ATTRIBS);
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_COMBINED_TEXTURE_IMAGE_UNITS);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MAX_TEXTURE_IMAGE_UNITS);
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &MAX_VERTEX_TEXTURE_IMAGE_UNITS);
        glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);

        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MAX_TEXTURE_SIZE);


    }

};

#endif //TOWERDEFENSE_RENDERINGCAPABILITIES_H
