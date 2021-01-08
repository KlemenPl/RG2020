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

    gui = new GUI(game->renderer2D);

    terrain = new Terrain();
}

void MainMenuScreen::show()
{
    Input::clearEventListiners();
    game->renderer3D->reset();
    game->renderer3D->setCamera(camera);
    game->renderer3D->addDirLight(DirLight(glm::vec3(0.5f, -0.5f, 0.0f)));
    srand(time(nullptr));
    uint64_t seed = rand();
    terrain->generate(75, 75, 2, 2, seed, 1,
                      ResourceManager::getBiome("defaultBiome")._get_ref());
    game->renderer3D->setTerrain(terrain);
    game->renderer3D->setSkybox(ResourceManager::getCubeMap("skybox")._get_ptr());

    gui->initInput();

}

MainMenuScreen::~MainMenuScreen()
{
    delete gui;
    delete camera;
    delete orthoCamera;
    delete terrain;
}
void MainMenuScreen::update(float dt)
{
    float rotationRadius = 42.0f;
    float rotationSpeed = 0.25f;
    float sinX = sinf(game->elapsedTime * rotationSpeed) * rotationRadius;
    float cosY = cosf(game->elapsedTime * rotationSpeed) * rotationRadius;
    //camera->setFront(glm::vec3(0,0,0));
    camera->setPosition(glm::vec3(sinX, 15, cosY));
    camera->lookAt(glm::vec3(0, 0, 0));

    camera->update();
    orthoCamera->update();
    game->renderer2D->setProjectionMatrix(orthoCamera->getCombined());

}
void MainMenuScreen::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game->renderer3D->begin();
    game->renderer3D->end();

    gui->begin();
    float w = (float) Game::width;
    float h = (float) Game::height;
    float hW = w / 2.0f;
    float hH = h / 2.0f;
    Ref<BitmapFont> bungeeFont = ResourceManager::getFont("bungee");
    Ref<BitmapFont> robotoFont = ResourceManager::getFont("roboto_black");
    std::string title = "Island Defense";
    float titleSize = bungeeFont->getTextWidth(title, 1.5f);
    gui->TextLabel(bungeeFont, title, glm::vec4(hW - titleSize / 2.0f, h * 0.82f, titleSize, h * 0.1f), 1.5f);

    float playWidth = robotoFont->getTextWidth("Play", 1.0f);
    float settingsWidth = robotoFont->getTextWidth("Settings", 1.0f);
    float exitWidth = robotoFont->getTextWidth("Exit", 1.0f);
    if (gui->TextButton(robotoFont, "Play",
                        glm::vec4(hW - playWidth / 2.0f, h * 0.62f, playWidth, h * 0.08f)).first)
    {
        game->setScreen(GAME_SCREEN);
    }
    if (gui->TextButton(robotoFont, "Settings",
                        glm::vec4(hW - settingsWidth / 2.0f, h * 0.52f, settingsWidth, h * 0.08f)).first)
    {
        // todo
    }
    if (gui->TextButton(robotoFont, "Exit",
                        glm::vec4(hW - exitWidth / 2.0f, h * 0.42f, exitWidth, h * 0.08f)).first)
    {
        game->setIsRunning(false);
    }
    gui->end();
}
void MainMenuScreen::resize(int width, int height)
{
    camera->resize(width, height);
    orthoCamera->resize(width, height);
}
