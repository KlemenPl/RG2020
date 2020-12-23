//
// Created by klemen on 14/12/2020.
//

#ifndef TOWERDEFENSE_EVENTS_H
#define TOWERDEFENSE_EVENTS_H

#include <functional>
#include "KeyActions.h"
#include "KeyCodes.h"

/**************
 * INTERFACES
 **************
 *
 * Interfaces consisting of pure
 * virtual function.
 *
 * Could probably be done a lot better, but
 * I am still very new to C++.
 */

class KeyEvent
{
public:
    const int keyCode;
    const int action;
    const std::function<bool()> callback;
    KeyEvent(const int keyCode, const int action, const std::function<bool()> &callback) :
            keyCode(keyCode), action(action),
            callback(callback)
    {}
};

class ButtonEvent
{
public:
    const int buttonCode;
    const int action;
    const std::function<bool(float, float)> callback;
    ButtonEvent(const int buttonCode, const int action, const std::function<bool(float, float)> &callback) :
            buttonCode(buttonCode), action(action),
            callback(callback)
    {}
};

class MouseMoveEvent
{
public:
    const std::function<bool(float mouseX, float mouseY, float deltaX, float deltaY)> callback;

    MouseMoveEvent(const std::function<bool(float, float, float, float)> &callback) :
            callback(callback)
    {}
};


class MouseScrollEvent
{
public:
    const std::function<bool(float amountX, float amountY)> callback;

    MouseScrollEvent(const std::function<bool(float, float)> &callback) :
            callback(callback)
    {}
};

class ResizeEvent
{
public:
    const std::function<bool(int newWidth, int newHeight)> callback;
    ResizeEvent(const std::function<bool(int, int)> &callback) : callback(callback)
    {}
};

#endif //TOWERDEFENSE_EVENTS_H
