//
// Created by klemen on 27/12/2020.
//

#include "GameScreen.h"
#include "../Game.h"
#include "../../engine/ResourceManager.h"

void GameScreen::init()
{
    camera = new PerspectiveCamera(Game::width, Game::height);
    orthoCamera = new OrthographicCamera(0, Game::width, 0, Game::height);

    uint32_t fbWidth = 128;
    uint32_t fbHeight = 128;

    previewCamera = new PerspectiveCamera(fbWidth, fbHeight);
    previewCamera->setPosition(glm::vec3(0, 1, 2));

    mousePicker = new MousePicker(*camera);

    game->renderer3D->setCamera(camera);
    game->renderer3D->addDirLight(DirLight(glm::vec3(0.5f, -0.5f, 0.0f)));
    game->renderer3D->addPointLight(PointLight(glm::vec3(4.0f, 30.0f, 5.0f),
                                               glm::vec3(1, 0, 0),
                                               glm::vec3(1, 0, 0),
                                               glm::vec3(1, 0, 0)
    ));

    gui = new GUI(game->renderer2D);

    terrain = new Terrain();
    Biome biome{};
    biome.colours.emplace_back(Color::create(201, 178, 99));
    biome.colours.emplace_back(Color::create(135, 184, 82));
    biome.colours.emplace_back(Color::create(80, 171, 93));
    biome.colours.emplace_back(Color::create(120, 120, 120));
    biome.colours.emplace_back(Color::create(200, 200, 210));
    //terrain->generate(50, 50, 2, 2,5,5, biome);

    previewTurret_0 = new Model(ResourceManager::getRawModel("turret_0")._get_ref());
    previewTurret_1 = new Model(ResourceManager::getRawModel("turret_1")._get_ref());

    turret_0 = new Model(ResourceManager::getRawModel("turret_0")._get_ref());
    turret_1 = new Model(ResourceManager::getRawModel("turret_1")._get_ref());

    previewFB1 = new FrameBuffer(fbWidth, fbHeight);
    previewFB2 = new FrameBuffer(fbWidth, fbHeight);

    previewFB1->createColourAttachment();
    previewFB2->createColourAttachment();

    // loading placement shader
    ResourceManager::loadShader("res/shaders/default3D_VS.glsl",
                                "res/shaders/placement_FS.glsl",
                                nullptr, "placement_shader");
    placementShader = ResourceManager::getShader("placement_shader");

    rawCubeModel = ResourceManager::getRawModel("cube")._get_ptr();

    Material enemyMaterial{};
    enemyMaterial.Ka = {0.5f, 0.0f, 0.0f};
    enemyMaterial.Kd = {0.82f, 0.2f, 0.2f};
    enemyMaterial.Ks = {0.95f, 0.4f, 0.4f};
    enemyMaterial.Ns = 255.0f;
    rawCubeModel->groups.front().meshMaterials.clear();
    rawCubeModel->groups.front().meshMaterials.push_back(enemyMaterial);

    glEnable(GL_DEPTH_TEST);

}

void GameScreen::show()
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
    mcc = new MouseCameraController(camera);
    mcc->setup();

    Input::addKeyEvent(KeyEvent(KEY_ESCAPE, PRESS, [this]() -> bool {
        if (gamestate == PAUSED)
            gamestate = PLAYING;
        else if (gamestate == PLAYING)
            gamestate = PAUSED;
        else if (gamestate == PLACING_TURRET)
            gamestate = PLAYING;
        return false;
    }));
    gui->initInput();

    generator = std::mt19937(seed * game->elapsedTime);
    randomAngle = std::uniform_real_distribution<>(0.0f, 6.28319f);
    randomHeight = std::uniform_real_distribution<>(terrain->waterLevel, terrain->maxHeight);
    randomDamage = std::uniform_real_distribution<>(0, 20.0f);
    randomDir = std::uniform_real_distribution<>(-1.0f, 1.0f);
    randomSpawnChance = std::uniform_real_distribution<>(0.0f, 1.0f);

    /*
    Input::addMouseButtonEvent(ButtonEvent(MOUSE_BUTTON_1, PRESS, [this](float mX, float mY) -> bool {
        if (gamestate == PLAYING)
        {
            // todo perform collision check on enemies
        }
        return false;
    }));
     */

    reset();
    gamestate = PLAYING;

}

void GameScreen::reset()
{
    balance = 0;
    eliminations = 0;
    hitpoints = 1000;

    for (auto &enemy:enemies)
        delete enemy;
    for (auto &turret:turrets)
        delete turret;
    for (auto &particle:particles)
        delete particle;

    enemies.clear();
    turrets.clear();
    particles.clear();

    elapsedTime = 0;

    gamestate = PLAYING;
}
void GameScreen::close()
{
    gamestate = PAUSED;
}

GameScreen::~GameScreen()
{
    delete camera;
    delete orthoCamera;
    delete previewCamera;
    delete terrain;
    delete previewFB1;
    delete previewFB2;
    delete gui;
}

void GameScreen::spawnEnemy()
{
    srand(game->elapsedTime);
    float radius = 80.0;
    float offset = 2.5f;
    int i = enemies.size();
    float angle = (float) i / (float) 1000 * 360.0f;
    float displacement = (rand() % (int) (2 * offset * 100)) / 100.0f - offset;
    float x = sin(angle) * radius + displacement;
    displacement = (rand() % (int) (2 * offset * 100)) / 100.0f - offset;
    float z = cos(angle) * radius + displacement;
    float y = randomHeight(generator);

    enemies.emplace_back(new Enemy{glm::vec3(x, y, z), rawCubeModel, enemyConfig,
                                   glm::vec3{randomAngle(generator),
                                             randomAngle(generator),
                                             randomAngle(generator)}});

}

void GameScreen::spawnParticles(const glm::vec3 &pos, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        glm::vec3 step = {randomDir(generator), randomDir(generator), randomDir(generator)};
        glm::vec3 rotation = {randomDir(generator), randomDir(generator), randomDir(generator)};
        particles.push_back(new Particle(rawCubeModel, pos, step * 10.0f, rotation * 10.0f,
                                         glm::vec3(-1.0f, -1.0f, -1.0f), 0.75f));
    }
}

void GameScreen::update(float dt)
{

    elapsedTime += dt;

    camera->update();
    orthoCamera->update();
    previewCamera->update();
    auto pair = Input::getMousePos();

    mousePicker->update(pair.first, pair.second);

    if (gamestate == PLAYING || gamestate == PLACING_TURRET)
    {
        if (enemies.size() < 1000)
        {
            if (((elapsedTime/2.0f) / 850.0f) > randomSpawnChance(generator))
                spawnEnemy();

        }


        // updating turrets
        auto itTurret = turrets.begin();
        while (itTurret != turrets.end())
        {
            auto &turret = *itTurret;
            turret->update(dt, enemies);

            Enemy *shotEnemy = turret->shoot();
            if (shotEnemy != nullptr)
            {
                spawnParticles(shotEnemy->getModelGroup()->position, 30);
                balance += 15;
                shotEnemy->dealDamage(turret->getDamage());
            }


            itTurret++;
        }

        // updating enemies
        auto itEnemy = enemies.begin();
        while (itEnemy != enemies.end())
        {
            auto &enemy = *itEnemy;

            if (enemy->toRemove)
            {
                spawnParticles(enemy->getModelGroup()->position, 50);
                delete enemy;
                itEnemy = enemies.erase(itEnemy);
                continue;
            }
            else
                itEnemy++;

            enemy->update(dt, game->elapsedTime);

            enemy->toRemove = enemy->hitGround(terrain);
            if (enemy->toRemove)
            {
                spawnParticles(enemy->getModelGroup()->position, 50); // additional 50
                hitpoints -= 10;
                continue;
            }

            if (Input::isMouseButtonJustDown(MOUSE_BUTTON_1) && gamestate == PLAYING)
            {
                // checking for collision
                /*
                pos = mousePicker->planePointIntersection(glm::vec3(enemy->getModelGroup()->position.x, 2,
                                                                   enemy->getModelGroup()->position.z),
                                                          glm::vec3(0, 1, 0));
                std::cout << "z: " <<enemy->getModelGroup()->position.z << std::endl;
                std::cout << "x: " << pos.x << "\ty: " << pos.y << "\tz: " << pos.z << std::endl;
                 */
                if (mousePicker->intersectsSphere(enemy->getModelGroup()->position, 1.5f))
                {
                    bool destroyed = enemy->dealDamage(45.0f);

                    if (!destroyed)
                        spawnParticles(enemy->getModelGroup()->position, 35);
                    else
                    {
                        enemy->toRemove = true;
                        eliminations++;
                        balance += 20;
                    }
                }
            }
        }

        // updating particles
        auto itParticle = particles.begin();
        while (itParticle != particles.end())
        {
            auto &particle = *itParticle;
            particle->update(dt);

            if (particle->finished())
            {
                delete particle;
                itParticle = particles.erase(itParticle);
            }
            else
                itParticle++;
        }
    }

    if (hitpoints <= 0)
        gamestate = GAME_OVER;

    //std::cout<<Input::isMouseButtonJustDown(MOUSE_BUTTON_1)<<std::endl;

    //std::cout << "x: " << pos.x << "\ty: " << pos.y << "\tz: " << pos.z << std::endl;

    //mousePicker->intersetctsPlane(glm::vec3(0,0,0),glm::vec3(0,1,0));
    //std::cout<<mousePicker->intersetctsPlane(glm::vec3(0,0,0),glm::vec3(0,1,0))<<std::endl;

}
void GameScreen::render()
{
    glViewport(0, 0, Game::width, Game::height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    game->renderer3D->setTerrain(terrain);
    game->renderer3D->setCamera(camera);

    game->renderer3D->begin();

    if (gamestate == PLACING_TURRET)
    {

        //auto pos = mousePicker->getCurrentRay();
        auto pos = mousePicker->planePointIntersection(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        if (selectedTurret == TURRET_0)
        {
            if (balance < turretPrice1)
                gamestate = PLAYING;
            turret_0->setTranslation(glm::vec3(pos.x, terrain->getHeight(pos.x, pos.z), pos.z));
            game->renderer3D->draw(turret_0, true, false,
                                   placementShader._get_ptr(), [this](Shader *shader) {
                        if (terrain->getHeight(turret_0->modelGroups.front().position.x,
                                               turret_0->modelGroups.front().position.z) > terrain->waterLevel)
                            shader->setUniform("colour", glm::vec4(0, 1, 0, 1));
                        else
                            shader->setUniform("colour", glm::vec4(1, 0, 0, 1));
                    });
        }
        else
        {
            if (balance < turretPrice2)
                gamestate = PLAYING;
            turret_1->setTranslation(glm::vec3(pos.x, terrain->getHeight(pos.x, pos.z), pos.z));
            game->renderer3D->draw(turret_1, true, false,
                                   placementShader._get_ptr(), [this](Shader *shader) {
                        if (terrain->getHeight(turret_1->modelGroups.front().position.x,
                                               turret_1->modelGroups.front().position.z) > terrain->waterLevel)
                            shader->setUniform("colour", glm::vec4(0, 1, 0, 1));
                        else
                            shader->setUniform("colour", glm::vec4(1, 0, 0, 1));
                    });
        }

        if (Input::isMouseButtonJustDown(MOUSE_BUTTON_1))
        {
            Turret *turret = nullptr;
            // placing turret
            if (selectedTurret == TURRET_0)
            {
                glm::vec3 tPos = turret_0->modelGroups.front().position;
                turret = new Turret(tPos, ResourceManager::getRawModel("turret_0")._get_ptr(),
                                    1.0f, 50, 50);
                balance -= turretPrice1;
            }
            else if (selectedTurret == TURRET_1)
            {
                glm::vec3 tPos = turret_1->modelGroups.front().position;
                turret = new Turret(tPos, ResourceManager::getRawModel("turret_1")._get_ptr(),
                                    0.25f, 20, 30);
                balance -= turretPrice2;
            }
            turrets.push_back(turret);
        }
        else if (Input::isMouseButtonDown(MOUSE_BUTTON_2))
            gamestate = PLAYING;
    }

    // drawing enemies
    for (auto &enemy:enemies)
        game->renderer3D->draw(enemy->getModelPtr(), true);

    // drawing particles
    for (auto &particle:particles)
        game->renderer3D->draw(particle->getModelPtr(), false);

    // drawing turrets
    for (auto &turret:turrets)
        game->renderer3D->draw(turret->getModelPtr(), true);

    game->renderer3D->end();

    // drawing turret previews
    game->renderer3D->setTerrain(nullptr);
    game->renderer3D->setCamera(previewCamera);

    previewFB1->bind();
    Color drawColor = balance >= turretPrice1 ? Colors::GREEN : Colors::RED;
    glClearColor(drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game->renderer3D->begin();
    game->renderer3D->draw(previewTurret_0, false);
    game->renderer3D->end();
    previewFB1->unbind();

    previewFB2->bind();
    drawColor = balance >= turretPrice2 ? Colors::GREEN : Colors::RED;
    glClearColor(drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game->renderer3D->begin();
    game->renderer3D->draw(previewTurret_1, false);
    game->renderer3D->end();
    previewFB2->unbind();

    game->renderer2D->setProjectionMatrix(orthoCamera->getCombined());

    gui->begin();

    Ref<BitmapFont> robotoFont = ResourceManager::getFont("roboto_black");
    float w = (float) Game::width;
    float h = (float) Game::height;
    float hW = w / 2.0f;
    float hH = h / 2.0f;

    Ref<Texture2D> fbTexture1(previewFB1->colourAttachment);
    Ref<Texture2D> fbTexture2(previewFB2->colourAttachment);

    auto r1 = gui->ImageButton(fbTexture1, glm::vec4(10, 10, 128, 128),
                               robotoFont, "  $" + std::to_string(turretPrice1), 0.6f);

    auto r2 = gui->ImageButton(fbTexture2, glm::vec4(148, 10, 128, 128),
                               robotoFont, "  $" + std::to_string(turretPrice2), 0.6f);

    if (r1.second)
        previewTurret_0->rotate(glm::vec3(0, 0.8f, 0));
    else
        previewTurret_0->setRotation(glm::vec3(0, 0, 0));

    if (r2.second)
        previewTurret_1->rotate(glm::vec3(0, 0.8f, 0));
    else
        previewTurret_1->setRotation(glm::vec3(0, 0, 0));

    if (r1.first | r2.first)
    {
        if (gamestate == PLACING_TURRET)
            gamestate = PLAYING;
        else
            gamestate = PLACING_TURRET;

        if (r1.first)
            selectedTurret = TURRET_0;
        else
            selectedTurret = TURRET_1;
    }

    if (gamestate == PAUSED || gamestate == GAME_OVER)
    {
        gui->Rect(Color{0.0f, 0.0f, 0.0f, 0.8f}, glm::vec4{0, 0, w, h});
    }

    if (gamestate == PAUSED)
    {
        float pausedWidth = robotoFont->getTextWidth("Paused", 1.5f);
        float resumeWidth = robotoFont->getTextWidth("Resume", 1.0f);
        float exitWidth = robotoFont->getTextWidth("Exit", 1.0f);
        gui->TextLabel(robotoFont, "Paused", glm::vec4(hW - pausedWidth / 2.0f, h * 0.72f,
                                                       pausedWidth, h * 0.08f), 1.5f);
        if (gui->TextButton(robotoFont, "Resume",
                            glm::vec4(hW - resumeWidth / 2.0f, h * 0.52f, resumeWidth, h * 0.08f)).first)
        {
            gamestate = PLAYING;
        }
        if (gui->TextButton(robotoFont, "Exit",
                            glm::vec4(hW - exitWidth / 2.0f, h * 0.42f, exitWidth, h * 0.08f)).first)
        {
            game->setScreen(MAIN_MENU_SCREEN);
        }
    }
    else if (gamestate == GAME_OVER)
    {
        float gameoverWidth = robotoFont->getTextWidth("GAME OVER", 1.5f);
        float restartWidth = robotoFont->getTextWidth("Restart", 1.0f);
        float exitWidth = robotoFont->getTextWidth("Exit", 1.0f);
        gui->TextLabel(robotoFont, "GAME OVER", glm::vec4(hW - gameoverWidth / 2.0f, h * 0.72f,
                                                          gameoverWidth, h * 0.08f), 1.5f);
        if (gui->TextButton(robotoFont, "Restart",
                            glm::vec4(hW - restartWidth / 2.0f, h * 0.52f, restartWidth, h * 0.08f)).first)
        {
            game->setScreen(GAME_SCREEN);
        }
        if (gui->TextButton(robotoFont, "Exit",
                            glm::vec4(hW - exitWidth / 2.0f, h * 0.42f, exitWidth, h * 0.08f)).first)
        {
            game->setScreen(MAIN_MENU_SCREEN);
        }
    }
    else
    {   // playing

        gui->Rect(Color{0.0f, 0.0f, 0.0f, 0.8f}, glm::vec4{0, 600, 350, 120});
        gui->TextLabel(robotoFont, "HP: " + std::to_string(hitpoints),
                       glm::vec4(5, h * 0.95f, w * 0.1f, h * 0.1f), 0.6f);
        gui->TextLabel(robotoFont, "Balance: $" + std::to_string(balance),
                       glm::vec4(5, h * 0.90f, w * 0.1f, h * 0.1f), 0.6f);
        gui->TextLabel(robotoFont, "Kills: " + std::to_string(eliminations),
                       glm::vec4(5, h * 0.85f, w * 0.1f, h * 0.1f), 0.6f);

    }

    gui->end();

    /*
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game->renderer2D->begin();
    glDisable(GL_CULL_FACE);
    game->renderer2D->draw(*robotoFont->bitmapTexture, glm::vec2(0, 512), glm::vec2(1024, 1024), 0, 0, 1.0f, 1.0f,
                           Colors::WHITE, 0.0f, false, false);
    game->renderer2D->end();
     */

}
void GameScreen::resize(int width, int height)
{
    orthoCamera->resize(width, height);
    camera->resize(width, height);
}
