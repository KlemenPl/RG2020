//
// Created by klemen on 12/12/2020.
//

#ifndef TOWERDEFENSE_GAME_H
#define TOWERDEFENSE_GAME_H

#include <unordered_map>

enum ScreenType : char
{
    GAME_SCREEN
};

class Game
{
private:
    GLFWwindow *window;


    std::unordered_map<ScreenType, ScreenType> screenMap;
public:

    Game();
    ~Game();

    bool init();
    void start();
    void run();
    void setScreen(ScreenType type);


};


#endif //TOWERDEFENSE_GAME_H
