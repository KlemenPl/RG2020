//
// Created by klemen on 14/12/2020.
//

#ifndef TOWERDEFENSE_SCREEN_H
#define TOWERDEFENSE_SCREEN_H

#include "../../engine/input/Input.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Game;

/******************
 * Abstract class
 ******************
 */
class Screen
{
protected:
    const Game *game;
    GLFWwindow *window;

public:
    Screen(const Game *game, GLFWwindow *window) : game(game), window(window)
    {};

    virtual ~Screen() = default;

    virtual void init() = 0;

    virtual void update(float dt) = 0;

    virtual void render() = 0;

    virtual void resize(int width, int height) = 0;

    /*
     * Called when screen is showed
     */
    virtual void show()
    {};

    /*
     * Called when screen is hidden
     */
    virtual void close()
    {};


};

#endif //TOWERDEFENSE_SCREEN_H
