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

enum ScreenType : char
{
    TEST,
    MAIN_MENU_SCREEN,
    SETTINGS_SCREEN,
    GAME_SCREEN,
    EDITOR_SCREEN,
    PARTICLE_EDITOR_SCREEN,
};

class Game
{
private:
    GLFWwindow *window;

    inline static Screen *currentScreen = nullptr;
    std::unordered_map<ScreenType, Screen *> screenMap;

    template<typename T>
    void addScreen(ScreenType screenType)
    {
        screenMap[screenType] = new T(this, window);
    }

    void initScreens();

public:

    Game();
    ~Game();

    bool init();
    void start();
    void run();
    void setScreen(ScreenType type);
    void setTitle(std::string newTitle);
};


#endif //TOWERDEFENSE_GAME_H
