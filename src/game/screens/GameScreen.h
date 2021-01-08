//
// Created by klemen on 27/12/2020.
//

#ifndef TOWERDEFENSE_GAMESCREEN_H
#define TOWERDEFENSE_GAMESCREEN_H


#include "Screen.h"
#include "../../engine/camera/PerspectiveCamera.h"
#include "../../engine/camera/OrthographicCamera.h"
#include "../../engine/graphics/Terrain.h"
#include "../../engine/graphics/FrameBuffer.h"
#include "../../engine/camera/MouseCameraController.h"
#include "../../engine/camera/MousePicker.h"
#include "../entity/Turret.h"
#include "../entity/Enemy.h"
#include "../entity/Particle.h"
#include "../ui/GUI.h"
#include <random>

enum GameState : uint8_t
{
    PLAYING,
    PLACING_TURRET,
    PAUSED,
    GAME_OVER
};

/*
 * Should probably use command patteren...
 */
enum SelectedTurret : uint8_t
{
    NONE,
    TURRET_0,
    TURRET_1,
};

class GameScreen : public Screen
{
private:
    PerspectiveCamera *camera;
    OrthographicCamera *orthoCamera;
    Terrain *terrain;

    MouseCameraController *mcc;
    MousePicker *mousePicker;

    const uint32_t turretPrice1 = 150;
    const uint32_t turretPrice2 = 750;

    PerspectiveCamera *previewCamera;

    Model *previewTurret_0;
    Model *previewTurret_1;

    Model *turret_0;
    Model *turret_1;

    FrameBuffer *previewFB1;
    FrameBuffer *previewFB2;

    GUI *gui;

    float elapsedTime{};

    Ref<Shader> placementShader;

    SelectedTurret selectedTurret = NONE;

    std::deque<Turret *> turrets{};
    std::deque<Enemy *> enemies{};
    std::deque<Particle *> particles{};

    std::mt19937 generator;
    std::uniform_real_distribution<> randomAngle;
    std::uniform_real_distribution<> randomHeight;
    std::uniform_real_distribution<> randomDamage;
    std::uniform_real_distribution<> randomDir;
    std::uniform_real_distribution<> randomSpawnChance;

    EnemyConfig enemyConfig{};
    RawModel *rawCubeModel{};

    uint32_t balance{};
    uint32_t eliminations{};
    int hitpoints{};

    GameState gamestate = PAUSED;

    void reset();

    void spawnEnemy();
    void spawnParticles(const glm::vec3 &pos, uint32_t count);
public:
    using Screen::Screen;

    virtual ~GameScreen() override;

    void init() override;
    void update(float dt) override;
    void render() override;
    void resize(int width, int height) override;
    void show() override;
    void close() override;

};


#endif //TOWERDEFENSE_GAMESCREEN_H
