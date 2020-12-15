//
// Created by klemen on 14/12/2020.
//

#ifndef TOWERDEFENSE_EVENTS_H
#define TOWERDEFENSE_EVENTS_H

/**************
 * INTERFACES
 **************
 *
 * Interfaces consisting of pure
 * virtual function.
 *
 * Could probably be done a lot nicer, but
 * I just started learnign c++.
 */

class KeyDownEvent
{
public:
    virtual bool keyDown(int key) = 0;
};

class KeyIsDownEvent
{
public:
    virtual bool keyIsDown(int key) = 0;
};

class KeyUpEvent
{
public:
    virtual bool keyUp(int key) = 0;
};

class MouseDownEvent
{
public:
    virtual bool mouseDown(int button) = 0;
};

class MouseIsDownEvent
{
public:
    virtual bool mouseIsDown(int button) = 0;
};

class MouseUpEvent
{
public:
    virtual bool mouseUp(int button) = 0;
};

class MouseMoveEvent
{
public:
    virtual bool mouseMove(float posX, float posY) = 0;
};

class MouseScrollEvent
{
public:
    virtual bool mouseScroll(float amountX, float amountY) = 0;
};

class ResizeEvent
{
public:
    virtual bool resized(int newWidth, int newHeight) = 0;
};

#endif //TOWERDEFENSE_EVENTS_H
