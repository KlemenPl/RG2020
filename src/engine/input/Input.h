//
// Created by klemen on 14/12/2020.
//

#ifndef TOWERDEFENSE_INPUT_H
#define TOWERDEFENSE_INPUT_H

#include <vector>
#include "KeyCodes.h"
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
    std::vector<KeyEvent> keyEvents;
    std::vector<ButtonEvent> buttonEvents;

    std::vector<MouseMoveEvent> mousemoveEvents;
    std::vector<MouseScrollEvent> mousescrollEvents;
    std::vector<ResizeEvent> resizeEvents;


    // keyboard key input
    int keys[KEY_LAST]{}; // 348 * 4 bytes

    // mouse button input
    int buttons[MOUSE_BUTTON_LAST]{}; // 7 * 4 bytes

    // mouse move input
    float mouseX{}, mouseY{};
    float lastMouseX{}, lastMouseY{};
    float deltaMouseX{}, deltaMouseY{};

public:
    static bool init();

    static void handleEvents();

    static void dispose();

    static void clearEventListiners();

    static void addKeyEvent(const KeyEvent &event);
    static void addMouseButtonEvent(const ButtonEvent &event);
    static void addMouseMoveEvent(const MouseMoveEvent &event);
    static void addMouseScrollEvent(const MouseScrollEvent &event);
    static void addResizeEvent(const ResizeEvent &event);


    // is key is continiously pressed down
    static bool isKeyDown(int key);
    // is key is just pressed down
    static bool isKeyJustDown(int key);
    // is key is just released
    static bool isKeyUp(int key);
    // is mouse button down
    static bool isMouseButtonDown(int button);
    // is mouse button just pressed
    static bool isMouseButtonJustDown(int button);
    // is mouse button just released
    static bool isMouseButtonUp(int button);
    // pair of mouse position
    static std::pair<float, float> getMousePos();
    // pair of mouse position changed
    static std::pair<float, float> getMouseDelta();


    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    static void WindowSizeCallback(GLFWwindow *window, int nWidth, int nHeight);

};

#endif //TOWERDEFENSE_INPUT_H
