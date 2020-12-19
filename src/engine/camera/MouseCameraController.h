//
// Created by klemen on 19/12/2020.
//

#ifndef TOWERDEFENSE_MOUSECAMERACONTROLLER_H
#define TOWERDEFENSE_MOUSECAMERACONTROLLER_H

#include "PerspectiveCamera.h"

class Input;

class MouseCameraController
{
private:
    PerspectiveCamera &camera;
public:
    explicit MouseCameraController(PerspectiveCamera &camera) : camera(camera)
    {}

    void setup(Input *inputInstance);
};


#endif //TOWERDEFENSE_MOUSECAMERACONTROLLER_H
