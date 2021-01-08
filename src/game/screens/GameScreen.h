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

class GameScreen : public Screen
{
private:
    PerspectiveCamera *camera;
    OrthographicCamera *orthoCamera;
    Terrain *terrain;

    MouseCameraController *mcc;
    MousePicker *mousePicker;

    RawModel *turret_0;
    RawModel *turret_1;

    Model *previewTurret_0;
    Model *previewTurret_1;

    FrameBuffer *previewFB;

    GUI *gui;

    Ref<Shader> placementShader;

    std::deque<Turret*> turrets{};
    std::deque<Enemy*> enemies{};
    std::deque<Particle*> particles{};

    std::mt19937 generator;
    std::uniform_real_distribution<> randomAngle;
    std::uniform_real_distribution<> randomHeight;
    std::uniform_real_distribution<> randomDamage;
    std::uniform_real_distribution<> randomDir;

    EnemyConfig enemyConfig{};
    RawModel *rawCubeModel{};

    uint32_t balance{};
    uint32_t eliminations{};
    int hitpoints{};

    GameState gamestate = PAUSED;

    void reset();

    void spawnEnemy();
    void spawnParticles(const glm::vec3 &pos, uint32_t count);
    void spawnBullet(const glm::vec3 &start, const glm::vec3 &target);
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
