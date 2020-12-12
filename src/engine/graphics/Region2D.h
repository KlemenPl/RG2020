//
// Created by klemen on 11/12/2020.
//

#ifndef TOWERDEFENSE_REGION2D_H
#define TOWERDEFENSE_REGION2D_H


#include <cstdint>
#include "UVRegion.h"

class Region2D : public UVRegion
{
    uint32_t ID;
    float U0;
    float V0;
    float U1;
    float V1;

public:
    Region2D(uint32_t id, float u0, float v0, float u1, float v1);
    uint32_t getID()const override;
    void bind() const override;
    float getU0() const override;
    float getV0() const override;
    float getU1() const override;
    float getV1() const override;
};


#endif //TOWERDEFENSE_REGION2D_H
