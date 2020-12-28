//
// Created by klemen on 27/12/2020.
//

#include <iostream>
#include "GameScreen.h"
#include "../Game.h"
#include "../../engine/ResourceManager.h"
#include "../../engine/camera/MouseCameraController.h"

MouseCameraController *mcc;

void GameScreen::init()
{
    camera = new PerspectiveCamera(Game::width, Game::height);
    orthoCamera = new OrthographicCamera(0, Game::width, 0, Game::height);

    game->renderer3D->setCamera(camera);
    game->renderer3D->addDirLight(DirLight(glm::vec3(0.0f, 1.0f, -0.2f)));

    mcc = new MouseCameraController(camera);
    mcc->setup();

    terrain = new Terrain();
    terrain->generate(500, 500, 20.0f, 123413, 10);

}

GameScreen::~GameScreen()
{
    delete camera;
    delete orthoCamera;
}

void GameScreen::update(float dt)
{

}
void GameScreen::render()
{
    glViewport(0, 0, Game::width, Game::height);
    glClearColor(0.1f, 0.1f, 0.25f, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    camera->update();
    orthoCamera->update();

    game->renderer3D->drawTerrain(*terrain);

    game->renderer3D->begin();
    game->renderer3D->end();

}
void GameScreen::resize(int width, int height)
{
    orthoCamera->resize(width, height);
    camera->resize(width, height);
}
