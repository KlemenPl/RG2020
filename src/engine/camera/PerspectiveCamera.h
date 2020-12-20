//
// Created by klemen on 12/12/2020.
//

#ifndef TOWERDEFENSE_PERSPECTIVECAMERA_H
#define TOWERDEFENSE_PERSPECTIVECAMERA_H

#include <ext.hpp>
#include "Camera.h"

// todo: start using quaternions, because of gimbal lock

class PerspectiveCamera : public Camera
{
private:
    glm::vec3 front;
    glm::vec3 right{};
    glm::vec3 up{};
    glm::vec3 worldUp;

    float yaw{};
    float pitch{};
    float roll{}; // todo: implement

    float FOV;

    bool updateProjection = false;

public:
    PerspectiveCamera(float width, float height, float fov = 45,
                      glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f),
                      glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f)) :
            front(front),
            worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
            FOV(fov)
    {
        this->position = pos;
        this->width = width;
        this->height = height;
        this->aspect = width / height;
        this->yaw = -90.0f;
        this->near = 0.1f;
        this->far = 100.0f;

        this->combined = glm::mat4(1.0f);
        this->projection = glm::mat4(1.0f);
        this->view = glm::mat4(1.0f);

        updateMatrix = true;
        updateProjection = true;
    }

    void update() override
    {
        if (updateMatrix)
        {
            front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
            front.y = glm::sin(glm::radians(pitch));
            front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
            front = glm::normalize(front);

            right = glm::normalize(glm::cross(front, worldUp));
            up = glm::normalize(glm::cross(right, front));

            view = glm::lookAt(position, position + front, up);
        }


        if (updateProjection)
        {
            projection = glm::perspective(glm::radians(FOV), aspect, near, far);
        }

        if (updateMatrix || updateProjection)
            combined =  projection * view;

        updateMatrix = false;
        updateProjection = false;
    }

    void resize(float screenX, float screenY) override
    {
        this->width = screenX;
        this->height = screenY;
        this->aspect = width / height;
        updateProjection = true;
    }

    void translateWFront(glm::vec3 &value)
    {
        translateWFront(value.x, value.y, value.z);
    }

    void translateWFront(float x, float y, float z)
    {
        position += front * z;
        position += right * x;
        position += up * y;

        updateMatrix = true;
    }

    void setFOV(float fov)
    {
        this->FOV = fov;

        updateProjection = true;
    }

    void setNear(float near)
    {
        this->near = near;

        updateProjection = true;
    }

    void setFar(float far)
    {
        this->far = far;

        updateProjection = true;
    }

    void setRotation(float _yaw, float _pitch)
    {
        this->yaw = _yaw;
        this->pitch = _pitch;

        updateMatrix = true;
    }

    float getFOV()
    {
        return FOV;
    }

    float getYaw() const
    {
        return yaw;
    }

    void setYaw(float _yaw)
    {
        this->yaw = _yaw;
        updateMatrix = true;
    }

    float getPitch() const
    {
        return pitch;
    }

    void setPitch(float _pitch)
    {
        this->pitch = _pitch;
        updateMatrix = true;
    }

    float getRoll() const
    {
        return roll;
    }

    void setRoll(float _roll)
    {
        this->roll = _roll;
        updateMatrix = true;
    }

};

#endif //TOWERDEFENSE_PERSPECTIVECAMERA_H
