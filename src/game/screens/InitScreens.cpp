//
// Created by klemen on 14/12/2020.
//
#include "../Game.h"
#include "TestScreen.h"
#include "GameScreen.h"

void Game::initScreens()
{
    addScreen<TestScreen>(TEST);
    addScreen<GameScreen>(GAME_SCREEN);

}
