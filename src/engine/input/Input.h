//
// Created by klemen on 14/12/2020.
//

#ifndef TOWERDEFENSE_INPUT_H
#define TOWERDEFENSE_INPUT_H

#include <vector>
#include "Keycodes.h"
#include "MouseButtonCodes.h"
#include "Events.h"
#include "KeyActions.h"

class GLFWwindow;

/*************
 * Singleton
 *************
 *
 * Probably way over engineered than it should be
 */
class Input
{
private:
    // private (singleton)
    Input() = default;

    inline static Input *instance = nullptr;

    /******************
     * Event processes
     ******************
     *
     * They are handled in FIFO order.
     * If an even returns true (meaning it has
     * been 'processed') it will stop calling
     * other events of the same type.
     */
    std::vector<KeyDownEvent> keydownEvents;
    std::vector<KeyIsDownEvent> keyisdownEvents;
    std::vector<KeyUpEvent> keyupEvents;

    std::vector<MouseDownEvent> mousedownEvents;
    std::vector<MouseIsDownEvent> mouseisdownEvents;
    std::vector<MouseUpEvent> mouseupEvenets;

    std::vector<MouseMoveEvent> mousemoveEvents;
    std::vector<MouseScrollEvent> mousescrollEvents;

    std::vector<ResizeEvent> resizeEvents;


    // keyboard key input
    int keys[KEY_LAST]; // 348 * 4 bytes

    // mouse button input
    int buttons[MOUSE_BUTTON_LAST]; // 7 * 4 bytes

    // mouse move input
    float mouseX, mouseY;
    float lastMouseX, lastMouseY;
    float deltaMouseX, deltaMouseY;

public:
    static bool init()
    {
        if (instance != nullptr)
            return false;

        instance = new Input();
        return true;
    }

    static void dispose()
    {
        delete instance;
    }

    static void clearEventListiners()
    {
        instance->keydownEvents.clear();
        instance->keyupEvents.clear();

        instance->mousedownEvents.clear();
        instance->mouseupEvenets.clear();

        instance->mousemoveEvents.clear();
        instance->mousescrollEvents.clear();

        instance->resizeEvents.clear();
    }

    static void addKeyDownEvent(KeyDownEvent event){
        instance->keydownEvents.push_back(event);
    }
    static void addKeyUpEvent(KeyUpEvent event){

    }

    static void addMouseDownEvent(MouseDownEvent event){

    }
    static void addMouseUpEvent(MouseUpEvent event){

    }

    static void addMouseMoveEvent(MouseMoveEvent event){

    }
    static void addMouseScrollEvent(MouseScrollEvent event){

    }

    static void addResizeEvent(ResizeEvent event){

    }

    // is key is continiously pressed down
    static bool isKeyDown(int key)
    {
        return instance->keys[key] == PRESS ||
               instance->keys[key] == REPEAT;
    }

    // is key is just pressed down
    static bool isKeyJustDown(int key)
    {
        return instance->keys[key] == PRESS;
    }

    // is key is just released
    static bool isKeyUp(int key)
    {
        return instance->keys[key] == RELEASE;
    }

    // is mouse button down
    static bool isMouseButtonDown(int button)
    {
        return instance->buttons[button] == PRESS ||
               instance->buttons[button] == REPEAT;
    }

    // is mouse button just pressed
    static bool isMouseButtonJustDown(int button)
    {
        return instance->buttons[button] == PRESS;
    }

    // is mouse button just released
    static bool isMouseButtonUp(int button)
    {
        return instance->buttons[button] == RELEASE;
    }

    // pair of mouse position
    static std::pair<float, float> getMousePos()
    {
        return std::pair(instance->mouseX, instance->mouseY);
    }

    // pair of mouse position changed
    static std::pair<float, float> getMouseDelta()
    {
        return std::pair(instance->deltaMouseX, instance->deltaMouseY);
    }

    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        instance->keys[key] = action;

        if (action == RELEASE)
            for (auto &it:instance->keyupEvents)
                if (it.keyUp(key))
                    break;

        if (action == PRESS)
            for (auto &it:instance->keydownEvents)
                if (it.keyDown(key))
                    break;

        if (action == PRESS || action == REPEAT)
            for (auto &it:instance->keyisdownEvents)
                if (it.keyIsDown(key))
                    return;
    }

    static void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
    {
        instance->lastMouseX = instance->mouseX;
        instance->lastMouseY = instance->mouseY;

        instance->mouseX = xpos;
        instance->mouseY = ypos;

        instance->deltaMouseX = instance->mouseX - instance->lastMouseX;
        instance->deltaMouseY = instance->mouseY - instance->lastMouseY;

        for (auto &it : instance->mousemoveEvents)
            if (it.mouseMove(xpos, ypos))
                break;


    }

    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        int actionBefore = instance->buttons[button];
        instance->buttons[button] = action;

        if (action == RELEASE)
            for (auto &it:instance->mouseupEvenets)
                if (it.mouseUp(button))
                    break;

        if (action == PRESS)
            for (auto &it:instance->mousedownEvents)
                if (it.mouseDown(button))
                    break;

        if (action == PRESS || action == REPEAT)
            for (auto &it:instance->mouseisdownEvents)
                if (it.mouseIsDown(button))
                    return;
    }

    static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        for (auto &it:instance->mousescrollEvents)
            if (it.mouseScroll(xoffset, yoffset))
                break;
    }

    static void WindowSizeCallback(GLFWwindow *window, int nWidth, int nHeight)
    {
        for (auto &it:instance->resizeEvents)
            if (it.resized(nWidth, nHeight))
                break;
    }

};

#endif //TOWERDEFENSE_INPUT_H
