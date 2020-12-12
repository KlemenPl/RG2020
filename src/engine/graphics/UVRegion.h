//
// Created by klemen on 11/12/2020.
//

#ifndef TOWERDEFENSE_UVREGION_H
#define TOWERDEFENSE_UVREGION_H

/*
 ************
 * INTERFACE
 ************
 */

class UVRegion
{

public:
    virtual void bind() const = 0;
    virtual uint32_t getID() const= 0;

    virtual float getU0() const = 0;
    virtual float getV0() const = 0;
    virtual float getU1() const= 0;
    virtual float getV1() const= 0;
};

#endif //TOWERDEFENSE_UVREGION_H
