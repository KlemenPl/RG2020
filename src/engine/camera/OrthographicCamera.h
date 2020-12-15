//
// Created by klemen on 09/12/2020.
//

#ifndef TOWERDEFENSE_ORTHOGRAPHICCAMERA_H
#define TOWERDEFENSE_ORTHOGRAPHICCAMERA_H

#include <ext.hpp>
#include "Camera.h"

class OrthographicCamera : public Camera
{
private:
    float rotation = 0;

    float left;
    float bottom;

public:
    OrthographicCamera(float left, float right, float bottom, float top, float near = -1.0, float far = 1.0)
    {
        this->left = left;
        this->bottom = bottom;
        //projection = math::ortho(left, right, bottom, top, near, far);
        //this->width = right;
        //this->height = top;
        //this->aspect=width/height;
        this->position = glm::vec3(0,0,0);
        resize(right,top);
    }

    void update() override
    {
        if (updateMatrix)
        {
            //view = math::translate(Mat4f(1.0f), position);
            //view = math::rotateZ(view, rotation);
            view = glm::translate(glm::mat4(1.0f),position);
            view = glm::rotate(view,glm::radians(rotation),glm::vec3(0,0,1));

            combined = view * projection;
        }
    }

    void setRotation(float newRotation)
    {
        this->rotation = newRotation;
        updateMatrix = true;
    }

    void resize(float screenX, float screenY) override
    {
        this->width=screenX;
        this->height=screenY;
        this->aspect=width/height;
        projection = glm::ortho(left, width, bottom, height, near, far);
        updateMatrix = true;
    }

    float getRotation() const { return rotation; }

};

#endif //TOWERDEFENSE_ORTHOGRAPHICCAMERA_H
