//
// Created by klemen on 11/12/2020.
//

#include <GL/gl.h>
#include "Region2D.h"

Region2D::Region2D(uint32_t id, float u0, float v0, float u1, float v1) : ID(id), U0(u0), V0(v0), U1(u1), V1(v1)
{}

uint32_t Region2D::getID() const
{
    return ID;
}
void Region2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D,ID);
}
float Region2D::getU0()const
{
    return U0;
}
float Region2D::getV0()const
{
    return V0;
}
float Region2D::getU1()const
{
    return U1;
}
float Region2D::getV1()const
{
    return V1;
}
