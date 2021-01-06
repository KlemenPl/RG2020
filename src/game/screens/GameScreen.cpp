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
    game->renderer3D->addDirLight(DirLight(glm::vec3(0.5f, -0.5f, 0.0f)));
    game->renderer3D->addPointLight(PointLight(glm::vec3(4.0f, 30.0f, 5.0f),
                                               glm::vec3(1,0,0),
                                               glm::vec3(1,0,0),
                                               glm::vec3(1,0,0)
                                               ));


    mcc = new MouseCameraController(camera);
    mcc->setup();

    terrain = new Terrain();
    Biome biome{};
    biome.colours.emplace_back(Color::create(201, 178, 99));
    biome.colours.emplace_back(Color::create(135, 184, 82));
    biome.colours.emplace_back(Color::create(80, 171, 93));
    biome.colours.emplace_back(Color::create(120, 120, 120));
    biome.colours.emplace_back(Color::create(200, 200, 210));
    //terrain->generate(50, 50, 2, 2,5,5, biome);

    glEnable(GL_DEPTH_TEST);
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
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    camera->update();
    orthoCamera->update();

    //game->renderer3D->begin();
    //game->renderer3D->end();

}
void GameScreen::resize(int width, int height)
{
    orthoCamera->resize(width, height);
    camera->resize(width, height);
}
