//
// Created by klemen on 09/12/2020.
//

#ifndef TOWERDEFENSE_CAMERA_H
#define TOWERDEFENSE_CAMERA_H


#include <mat4x4.hpp>

class Camera
{
protected:
    glm::vec3 position;

    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 combined; // view * proj

    float width;
    float height;
    float aspect;

    float near;
    float far;

    bool updateMatrix = true;

public:

    virtual void update() = 0; // pure virtual method

    void translate(const glm::vec3 &value)
    {
        translate(value.x, value.y, value.z);
    }

    // moves camera position
    void translate(float x, float y, float z)
    {
        position.x -= x; // opengl is right hand system
        position.y += y;
        position.z += z;
        updateMatrix = true;
    }

    void setPosition(const glm::vec3 &newPosition)
    {
        this->position = newPosition;
        updateMatrix = true;
    }


    virtual void resize(float screenX, float screenY) = 0;

    const glm::vec3 &getPosition() const { return position; }

    const glm::mat4 &getProjectionMatrix() const { return projection; };
    const glm::mat4 &getViewMatrix() const { return view; };
    const glm::mat4 &getCombined() const { return combined; };

    float getWidth() const
    {
        return width;
    }
    float getHeight() const
    {
        return height;
    }

};

#endif //TOWERDEFENSE_CAMERA_H
