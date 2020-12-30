//
// Created by klemen on 29/12/2020.
//

#include "TerrainTest.h"
#include "../engine/input/Input.h"
#include <chrono>

#define chrono_now() std::chrono::high_resolution_clock::now()

Biome *activeBiome;
Biome *tempBiome;

uint32_t *activeSeed;
uint32_t *tempSeed;

void TerrainTest::init()
{

    this->renderer3D = new Renderer3D();
    this->camera = new PerspectiveCamera(1280, 720);
    this->renderer3D->setCamera(camera);
    this->renderer3D->addDirLight(DirLight(glm::vec3(0.2f, -0.5f, 0.0f)));

    activeSeed = new uint32_t;
    *activeSeed = 8462;
    tempSeed = new uint32_t;
    *tempSeed = *activeSeed;

    terrain = new Terrain();
    activeBiome = new Biome();
    float terrainDivisor = 2.0f;
    activeBiome->colours.emplace_back(Color::create(201 / terrainDivisor, 178 / terrainDivisor, 99 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(201 / terrainDivisor, 178 / terrainDivisor, 99 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(135 / terrainDivisor, 184 / terrainDivisor, 82 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(135 / terrainDivisor, 184 / terrainDivisor, 82 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(80 / terrainDivisor, 171 / terrainDivisor, 93 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(120 / terrainDivisor, 120 / terrainDivisor, 120 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(200 / terrainDivisor, 200 / terrainDivisor, 210 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(200 / terrainDivisor, 200 / terrainDivisor, 210 / terrainDivisor));
    terrain->generate(20, 20, 5, 5, *activeSeed, 1, *activeBiome);
    tempBiome = new Biome(*activeBiome);

    Input::init();
    // setting up input callbacks
    glfwSetKeyCallback(window, Input::KeyCallback);
    glfwSetCursorPosCallback(window, Input::CursorPositionCallback);
    glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
    glfwSetScrollCallback(window, Input::ScrollCallback);
    glfwSetWindowSizeCallback(window, Input::WindowSizeCallback);

    this->mcc = new MouseCameraController(this->camera);
    this->mcc->setup();


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

}
void TerrainTest::start()
{
    while (!glfwWindowShouldClose(window))
    {
        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        Input::handleEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        camera->resize(display_w, display_h);
        //camera->setPosition(glm::vec3(0, 0, -3));
        camera->update();
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.25f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float startTime = glfwGetTime();

        if (((activeBiome && tempBiome) && *activeBiome != *tempBiome)
            || *activeSeed != *tempSeed)
        {
            delete activeBiome;
            activeBiome = tempBiome;
            *activeSeed = *tempSeed;
            terrain->generate(20, 20, 5, 5,  *activeSeed, 1, *activeBiome);
            tempBiome = new Biome(*activeBiome);
        }

        auto start = chrono_now();
        renderer3D->drawTerrain(*terrain);
        renderer3D->begin();
        renderer3D->end();
        auto end = chrono_now();

        std::chrono::duration<float> duration = end - start;
        float fDuration = duration.count() * 1000.0f; // to ms


        startTime = glfwGetTime() - startTime;
        startTime *= 1000;


        ImGui::Begin("Terrain info");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        ImGui::Text("Game loop: %.3f ms.", deltaTime *= 1000);
        ImGui::Text("Drawin terrain: %.3f ms.", fDuration);
        ImGui::Text("");
        ImGui::SliderInt("Seed: %d.", (int *) tempSeed, 0, 10000);
        ImGui::SliderInt("Octaves", (int *) &tempBiome->octaves, 0, 20);
        ImGui::SliderFloat("Persistance", &tempBiome->persistance, 0, 5);
        ImGui::SliderFloat("Lacunarity", &tempBiome->lacunarity, 0, 5);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    delete renderer3D;
    delete camera;
    delete terrain;
    ResourceManager::dispose();

}
