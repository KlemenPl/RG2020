//
// Created by klemen on 06/01/2021.
//

#include "MainMenuScreen.h"
#include "../Game.h"
#include "../../engine/ResourceManager.h"

void MainMenuScreen::init()
{
    camera = new PerspectiveCamera(Game::width, Game::height);
    orthoCamera = new OrthographicCamera(0, Game::width, 0, Game::height);

    terrain = new Terrain();
}

void MainMenuScreen::show()
{
    game->renderer3D->reset();
    game->renderer3D->setCamera(camera);
    game->renderer3D->addDirLight(DirLight(glm::vec3(0.5f, -0.5f, 0.0f)));
    srand(time(nullptr));
    uint64_t seed = rand();
    terrain->generate(75, 75, 2, 2, seed, 1,
                      ResourceManager::getBiome("defaultBiome")._get_ref());
    game->renderer3D->setTerrain(terrain);
    game->renderer3D->setSkybox(ResourceManager::getCubeMap("skybox")._get_ptr());

}


MainMenuScreen::~MainMenuScreen()
{
    delete camera;
    delete orthoCamera;
    delete terrain;
}
void MainMenuScreen::update(float dt)
{
    float rotationRadius = 42.0f;
    float rotationSpeed = 0.35f;
    float sinX = sinf(game->elapsedTime * rotationSpeed) * rotationRadius;
    float cosY = cosf(game->elapsedTime * rotationSpeed) * rotationRadius;
    //camera->setFront(glm::vec3(0,0,0));
    camera->setPosition(glm::vec3(sinX, 15, cosY));
    camera->lookAt(glm::vec3(0,0,0));


    camera->update();
    orthoCamera->update();
    game->renderer2D->setProjectionMatrix(orthoCamera->getCombined());
}
void MainMenuScreen::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game->renderer3D->begin();
    game->renderer3D->end();
}
void MainMenuScreen::resize(int width, int height)
{
    camera->resize(width, height);
    orthoCamera->resize(width, height);
}
