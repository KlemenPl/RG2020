//
// Created by klemen on 19/12/2020.
//

#include "MouseCameraController.h"
#include "../input/Input.h"

void MouseCameraController::setup()
{
    std::function<bool(float, float, float, float)> mouseMove =
            [this](float mouseX, float mouseY, float deltaX, float deltaY) -> bool {
                if (Input::isMouseButtonDown(MOUSE_BUTTON_1))
                {
                    //std::cout << deltaX << std::endl;
                    float scale = 0.01f * (std::abs(deltaX) + std::abs(deltaY));
                    this->camera->translateWFront(-deltaX * scale, deltaY * scale, 0);
                }
                else if (Input::isMouseButtonDown(MOUSE_BUTTON_2))
                {
                    float scale = 0.5f * (std::abs(deltaX) + std::abs(deltaY));
                    this->camera->setYaw(camera->getYaw() + (-deltaX * scale));
                    this->camera->setPitch(camera->getPitch() + deltaY * scale);

                    //scale*=0.2f;
                    //glm::vec3 pos = this->camera->getPosition();
                    //pos.x+=deltaX*scale;
                    //pos.y+=-deltaY*scale;
                    //this->camera->setPosition(pos);
                }
                return true;
            };

    std::function<bool(float, float)> mosuseScroll =
            [this](float amountX, float amountY) -> bool {
                this->camera->translateWFront(0, 0, amountY);
                return true;
            };

    Input::addMouseMoveEvent(MouseMoveEvent(mouseMove));
    Input::addMouseScrollEvent(MouseScrollEvent(mosuseScroll));

    float moveSpeed = 0.08f;

    Input::addKeyEvent(KeyEvent(KEY_W, PRESS | REPEAT, [this, moveSpeed]() -> bool {
        this->camera->translateWFront(0, 0, moveSpeed);
        return false;
    }));
    Input::addKeyEvent(KeyEvent(KEY_S, PRESS | REPEAT, [this, moveSpeed]() -> bool {
        this->camera->translateWFront(0, 0, -moveSpeed);
        return false;
    }));
    Input::addKeyEvent(KeyEvent(KEY_A, PRESS | REPEAT, [this, moveSpeed]() -> bool {
        this->camera->translateWFront(-moveSpeed, 0, 0);
        return false;
    }));
    Input::addKeyEvent(KeyEvent(KEY_D, PRESS | REPEAT, [this, moveSpeed]() -> bool {
        this->camera->translateWFront(moveSpeed, 0, 0);
        return false;
    }));
    Input::addKeyEvent(KeyEvent(KEY_SPACE, PRESS | REPEAT, [this, moveSpeed]() -> bool {
        this->camera->translateWFront(0, moveSpeed, 0);
        return false;
    }));
    Input::addKeyEvent(KeyEvent(KEY_LEFT_CONTROL, PRESS | REPEAT, [this, moveSpeed]() -> bool {
        this->camera->translateWFront(0, -moveSpeed, 0);
        return false;
    }));
}
