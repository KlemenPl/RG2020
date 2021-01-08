//
// Created by klemen on 20/12/2020.
//

#include "Input.h"
#include <imgui.h>

bool Input::init()
{
    if (instance != nullptr)
        return false;

    instance = new Input();
    return true;
}

void Input::dispose()
{
    delete instance;
}

void Input::handleEvents()
{
    if (!instance->handleInput)
        return;

    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {
        // handling mouse events
        for (auto &it:instance->mousemoveEvents)
        {
            if (instance->deltaMouseX != 0.0 || instance->deltaMouseY != 0.0)
                if (it.callback(instance->mouseX, instance->mouseY,
                                instance->deltaMouseX, instance->deltaMouseY))
                    break;
        }
    }

    instance->deltaMouseX = 0;
    instance->deltaMouseY = 0;
}

void Input::reset()
{
    for (int i = 0; i < MOUSE_BUTTON_LAST; i++)
    {
        instance->buttonsJustDown[i] = 0;
    }
}

void Input::clearEventListiners()
{
    instance->keyEvents.clear();
    instance->buttonEvents.clear();

    instance->mousemoveEvents.clear();
    instance->mousescrollEvents.clear();

    instance->resizeEvents.clear();
}

void Input::addKeyEvent(const KeyEvent &event)
{
    instance->keyEvents.push_back(event);
}
void Input::addMouseButtonEvent(const ButtonEvent &event)
{
    instance->buttonEvents.push_back(event);
}

void Input::addMouseMoveEvent(const MouseMoveEvent &event)
{
    instance->mousemoveEvents.push_back(event);
}
void Input::addMouseScrollEvent(const MouseScrollEvent &event)
{
    instance->mousescrollEvents.push_back(event);
}
void Input::addResizeEvent(const ResizeEvent &event)
{
    instance->resizeEvents.push_back(event);
}

// is key is continiously pressed down
bool Input::isKeyDown(int key)
{
    return instance->keys[key] == PRESS ||
           instance->keys[key] == REPEAT;
}

// is key is just released
bool Input::isKeyUp(int key)
{
    return instance->keys[key] == RELEASE;
}

bool Input::isMouseButtonDown(int button)
{
    return instance->buttons[button] == PRESS ||
           instance->buttons[button] == REPEAT;
}

bool Input::isMouseButtonUp(int button)
{
    return instance->buttons[button] == RELEASE;
}

std::pair<float, float> Input::getMousePos()
{
    return std::pair(instance->mouseX, instance->mouseY);
}

std::pair<float, float> Input::getMouseDelta()
{
    return std::pair(instance->deltaMouseX, instance->deltaMouseY);
}

void Input::setHandleInput(bool handleInput)
{
    Input::handleInput = handleInput;
}

bool Input::isHandleInput() const
{
    return handleInput;
}

void Input::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    instance->keys[key] = action;
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard)
    {
        // handling key events
        for (auto &it:instance->keyEvents)
        {
            if (instance->keys[it.keyCode] & it.action)
            {
                if (it.callback())
                    break;
            }
        }
    }
}

bool Input::isMouseButtonJustDown(int button)
{
    return instance->buttonsJustDown[button] == PRESS;
}

void Input::CursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
    xpos = (float) xpos;
    ypos = (float) ypos;
    instance->lastMouseX = instance->mouseX;
    instance->lastMouseY = instance->mouseY;

    instance->mouseX = xpos;
    instance->mouseY = ypos;

    instance->deltaMouseX = instance->mouseX - instance->lastMouseX;
    instance->deltaMouseY = instance->mouseY - instance->lastMouseY;

}

void Input::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    instance->buttons[button] = action;
    instance->buttonsJustDown[button] = action;
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard)
    {
        // handling button events
        for (auto &it:instance->buttonEvents)
        {
            if (instance->buttons[it.buttonCode] & it.action)
            {
                if (it.callback(instance->mouseX, instance->mouseY))
                    break;
            }
        }
    }
}

void Input::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    for (auto &it:instance->mousescrollEvents)
        if (it.callback(xoffset, yoffset))
            break;
}

void Input::WindowSizeCallback(GLFWwindow *window, int nWidth, int nHeight)
{
    for (auto &it:instance->resizeEvents)
        if (it.callback(nWidth, nHeight))
            break;
}