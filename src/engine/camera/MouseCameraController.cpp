//
// Created by klemen on 19/12/2020.
//

#include "MouseCameraController.h"
#include "../input/Input.h"

void MouseCameraController::setup(Input *inputInstance)
{
    inputInstance->addMouseMoveEvent();
}
