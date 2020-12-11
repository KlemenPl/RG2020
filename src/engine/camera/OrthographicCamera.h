//
// Created by klemen on 09/12/2020.
//

#ifndef TOWERDEFENSE_ORTHOGRAPHICCAMERA_H
#define TOWERDEFENSE_ORTHOGRAPHICCAMERA_H

#include "../math/Vector.h"
#include "../math/MatrixMath.h"

class OrthographicCamera
{
private:
    Mat4f projectionMatrix;
    Mat4f viewMatrix;
    Mat4f combined; // (view * projection)

    Vec3f position;
    float rotation = 0;

    bool recalculate =  true;
public:
    OrthographicCamera(float left, float right, float bottom, float top):
    projectionMatrix(math::ortho(left,right,bottom,top,-1.0,1.0))
    {}

    void update()
    {
        Mat4f transform = math::translate(Mat4f(1.0f), position);
        transform = math::rotateZ(transform, rotation);

        //viewMatrix = math::inverse(transform);
        viewMatrix = transform;
        combined =  viewMatrix*projectionMatrix;
    }

    void setPosition(const Vec3f &newPosition) {
        this->position = newPosition;
        recalculate=true;
    }
    void setRotation(float newRotation) {
        this->rotation = newRotation;
        recalculate=true;
    }

    const Vec3f &getPosition() const { return position; }
    float getRotation() const { return rotation; }

    const Mat4f& getProjectionMatrix() const { return projectionMatrix; };
    const Mat4f& getViewMatrix() const { return viewMatrix; };
    const Mat4f& getCombined() const { return combined; };


};

#endif //TOWERDEFENSE_ORTHOGRAPHICCAMERA_H
