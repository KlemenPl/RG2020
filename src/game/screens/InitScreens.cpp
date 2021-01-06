//
// Created by klemen on 14/12/2020.
//
#include "../Game.h"
#include "GameScreen.h"
#include "MainMenuScreen.h"

void Game::initScreens()
{
    addScreen<MainMenuScreen>(MAIN_MENU_SCREEN);
    addScreen<GameScreen>(GAME_SCREEN);

}
