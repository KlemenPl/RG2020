//
// Created by klemen on 07/01/2021.
//

#ifndef TOWERDEFENSE_MOUSEPICKER_H
#define TOWERDEFENSE_MOUSEPICKER_H

#include <glm.hpp>
#include <iostream>
#include <gtx/intersect.hpp>
#include "PerspectiveCamera.h"

// https://antongerdelan.net/opengl/raycasting.html
class MousePicker
{
public:
    glm::vec3 ray;
    glm::vec3 rayOrigin;
private:

    const PerspectiveCamera &camera;

    glm::vec3 calculateMouseRay(float mouseX, float mouseY)
    {
        glm::vec2 ndc = toNDC(mouseX, camera.getHeight() - mouseY);
        glm::vec4 clipCoords{ndc.x, ndc.y, -1.0f, 1.0f};
        glm::vec4 eyeCoords = toEyeCoords(clipCoords);
        glm::vec3 worldRay = toWorldCoords(eyeCoords);
        return worldRay;
    }

    glm::vec3 toWorldCoords(const glm::vec4 &eyeCoords)
    {
        glm::vec3 rayWorld = glm::vec3(glm::inverse(camera.getViewMatrix()) * eyeCoords);
        return glm::normalize(glm::vec3{rayWorld.x, rayWorld.y, rayWorld.z});
    }

    glm::vec4 toEyeCoords(const glm::vec4 &clipCoords)
    {
        glm::mat4 inverted = glm::inverse(camera.getProjectionMatrix());
        glm::vec4 eyeCoords = inverted * clipCoords;
        return {eyeCoords.x, eyeCoords.y, -1.0f, 0.0f};
    }

    glm::vec2 toNDC(float x, float y)
    {
        return glm::vec2{(2.0f * x) / camera.getWidth() - 1.0f,
                         (2.0f * y) / camera.getHeight() - 1.0f};
    }

public:
    MousePicker(const PerspectiveCamera &camera) : camera(camera)
    {}

    const glm::vec3 &getCurrentRay()
    {
        return ray;
    }

    void update(float mouseX, float mouseY)
    {
        rayOrigin = camera.getPosition();
        ray = calculateMouseRay(mouseX, mouseY);
    }

    bool intersetctsPlane(const glm::vec3 &origin, const glm::vec3 &norm)
    {
        float denom = glm::dot(norm, ray);
        const float epsilon = 0.1f;
        if (std::abs(denom) > epsilon)
        {
            float t = glm::dot(origin - glm::normalize(camera.getPosition()), norm) / denom;
            if (t > epsilon) return true;
        }
        return false;

    }

    glm::vec3 planePointIntersection(const glm::vec3 &origin, const glm::vec3 &norm)
    {
        // camera.getPosition() is ray origin
        glm::vec3 diff = camera.getPosition() - origin;
        float p1 = glm::dot(diff, norm);
        float p2 = glm::dot(ray, norm);
        float p3 = p1 / p2;
        return camera.getPosition() - ray * p3;
    }

    // https://www.gamedev.net/forums/topic/262342-ray-sphere-intersection/
    bool intersectsSphere(const glm::vec3 &sphereOrigin, float radius)
    {
        glm::vec3 v1 = sphereOrigin - rayOrigin;
        glm::vec3 v2 = rayOrigin + ray * glm::length(v1);
        glm::vec3 v3 = v2 - sphereOrigin;

        return glm::length(v3) < radius;

        /* Java:
         * Vector3f v1 = sphereOrigin.Sub(rayOrigin);
         * Vector3f v2 = (rayOrigin.Add(ray)).Mul(v1.length());
         * Vector3f v3 = v2.Sub(sphereOrigin);
         */
    }


};

#endif //TOWERDEFENSE_MOUSEPICKER_H
