//
// Created by klemen on 12/12/2020.
//

#ifndef TOWERDEFENSE_GAME_H
#define TOWERDEFENSE_GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <unordered_map>
#include "screens/Screen.h"
#include "../engine/rendering/Renderer2D.h"
#include "../engine/rendering/Renderer3D.h"

enum ScreenType : char
{
    TEST,
    MAIN_MENU_SCREEN,
    GAME_SCREEN,
    PARTICLE_EDITOR_SCREEN,
};

class Game
{
private:
    GLFWwindow *window;
    bool isRunning = true;

    inline static Screen *currentScreen = nullptr;
    std::unordered_map<ScreenType, Screen *> screenMap;

    template<typename T>
    void addScreen(ScreenType screenType)
    {
        screenMap[screenType] = new T(this, window);
    }

    void initScreens();

public:
    Renderer2D* renderer2D;
    Renderer3D* renderer3D;

    inline static int width = 0;
    inline static int height = 0;

    Game();
    ~Game();

    bool init();
    void start();
    void run();
    void setScreen(ScreenType type);
    void setTitle(std::string newTitle);
    void setIsRunning(bool _isRunning);
    float elapsedTime = 0;
};


#endif //TOWERDEFENSE_GAME_H
