//
// Created by klemen on 09/12/2020.
//

#ifndef TOWERDEFENSE_CAMERA_H
#define TOWERDEFENSE_CAMERA_H

#include "../math/Matrix.h"
#include "../math/Vector.h"

class Camera
{
protected:
    Vec3f position;

    Mat4f view;
    Mat4f projection;
    Mat4f combined;

    float width;
    float height;

    float near;
    float far;

    bool updateProjMatrix = true;
    bool updateRotation = true;

public:

    virtual void update() = 0; // abstract method

    void translate(Vec3f &value)
    {
        translate(value.x, value.y, value.z);
    }

    // moves camera position
    void translate(float x, float y, float z)
    {
        position.x += x;
        position.y += y;
        position.z += z;
    }


    virtual void resize(float screenX, float screenY)=0;

};

#endif //TOWERDEFENSE_CAMERA_H
