//
// Created by klemen on 19/12/2020.
//

#ifndef TOWERDEFENSE_MOUSECAMERACONTROLLER_H
#define TOWERDEFENSE_MOUSECAMERACONTROLLER_H

#include "PerspectiveCamera.h"

class Input;

/*
 * Arc-ball camera contoller
 */
class MouseCameraController
{
private:
    PerspectiveCamera *camera;
    float totalRotation = 0;
    float zoom = 40.0f;

    void updateCamera(float deltaX, float deltaY);
    void updateZoom(float delta);
public:
    explicit MouseCameraController(PerspectiveCamera *camera) : camera(camera)
    {}

    void setup();
};


#endif //TOWERDEFENSE_MOUSECAMERACONTROLLER_H
