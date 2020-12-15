//
// Created by klemen on 12/12/2020.
//

#ifndef TOWERDEFENSE_PERSPECTIVECAMERA_H
#define TOWERDEFENSE_PERSPECTIVECAMERA_H

#include "Camera.h"

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
        updateMatrix = true;
    }

    void update() override
    {
        if (updateMatrix)
        {
            front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
            front.y = sinf(glm::radians(pitch));
            front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
            front = glm::normalize(front);

            updateMatrix = false;
        }

        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));

        view = glm::lookAt(position, position + front, up);

        if (updateProjection)
        {
            projection = glm::perspective(FOV, aspect, near, far);
            updateProjection = false;
        }

        combined = view * projection;
    }

    void resize(float screenX, float screenY) override
    {
        this->width = screenX;
        this->height = screenY;
        this->aspect = width / height;
        updateMatrix = true;
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

        updateProjection = true;
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
        updateProjection = true;
    }

    float getPitch() const
    {
        return pitch;
    }

    void setPitch(float _pitch)
    {
        this->pitch = _pitch;
        updateProjection = true;
    }

    float getRoll() const
    {
        return roll;
    }

    void setRoll(float _roll)
    {
        this->roll = _roll;
        updateProjection = true;
    }

};

#endif //TOWERDEFENSE_PERSPECTIVECAMERA_H
