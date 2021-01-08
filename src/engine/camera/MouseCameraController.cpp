//
// Created by klemen on 19/12/2020.
//

#include "MouseCameraController.h"
#include "../input/Input.h"

void MouseCameraController::updateCamera(float deltaX, float deltaY)
{
    this->camera->translate(0, deltaY, 0);

    totalRotation -= deltaX;
    float angle = glm::radians(totalRotation);
    float sinX = sinf(angle) * zoom;
    float cosY = cosf(angle) * zoom;

    glm::vec3 pos = camera->getPosition();
    pos.x = sinX;
    pos.z = cosY;
    camera->setPosition(pos);
    camera->lookAt(glm::vec3(0, 0, 0));
}
void MouseCameraController::updateZoom(float delta)
{
    zoom -= delta;
    if (zoom < 10)
        zoom = 10;
    else if (zoom > 65)
        zoom = 65;
    updateCamera(0, 0);
}
void MouseCameraController::setup()
{
    std::function<bool(float, float, float, float)> mouseMove =
            [this](float mouseX, float mouseY, float deltaX, float deltaY) -> bool {
                if (!enabled)
                    return false;
                if (Input::isMouseButtonDown(MOUSE_BUTTON_2))
                {
                    updateCamera(deltaX, deltaY);
                }
                return false;
            };

    Input::addMouseMoveEvent(MouseMoveEvent(mouseMove));

    std::function<bool(float, float)> mosuseScroll =
            [this](float amountX, float amountY) -> bool {
                if (!enabled)
                    return false;
                updateZoom(amountY * 1.5f);
                return false;
            };
    Input::addMouseScrollEvent(MouseScrollEvent(mosuseScroll));


    float moveSpeed = 0.4f;

    Input::addKeyEvent(KeyEvent(KEY_W, PRESS | REPEAT, [this, moveSpeed]() -> bool {
        if (!enabled)
            return false;
        updateZoom(moveSpeed);
        return false;
    }));
    Input::addKeyEvent(KeyEvent(KEY_S, PRESS | REPEAT, [this, moveSpeed]() -> bool {
        if (!enabled)
            return false;
        updateZoom(-moveSpeed);
        return false;
    }));
    Input::addKeyEvent(KeyEvent(KEY_A, PRESS | REPEAT, [this, moveSpeed]() -> bool {
        if (!enabled)
            return false;
        updateCamera(moveSpeed, 0);
        return false;
    }));
    Input::addKeyEvent(KeyEvent(KEY_D, PRESS | REPEAT, [this, moveSpeed]() -> bool {
        if (!enabled)
            return false;
        updateCamera(-moveSpeed, 0);
        return false;
    }));
    updateCamera(0, 0);

    enabled = true;
}
