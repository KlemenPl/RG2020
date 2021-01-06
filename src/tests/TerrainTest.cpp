//
// Created by klemen on 29/12/2020.
//

#include "TerrainTest.h"
#include "../engine/input/Input.h"
#include "../engine/loader/Loader.h"
#include "../game/Game.h"
#include <chrono>

#define chrono_now() std::chrono::high_resolution_clock::now()

Biome *activeBiome;
Biome *tempBiome;

uint32_t *activeSeed;
uint32_t *tempSeed;

FrameBuffer *testFrameBuffer;

void TerrainTest::init()
{

    Game::width = 1280;
    Game::height = 720;
    this->renderer3D = new Renderer3D();
    this->camera = new PerspectiveCamera(1280, 720);

    this->renderer2D = new Renderer2D();

    this->renderer3D->setCamera(camera);
    this->renderer3D->addDirLight(DirLight(glm::vec3(0.2f, -0.5f, 0.0f)));

    ShaderSourceArgument args[1];
    args[0] = ShaderSourceArgument{FRAGMENT, "TEXTURE_SLOTS",
                                   std::to_string(RenderingCapabilities::MAX_TEXTURE_IMAGE_UNITS)};
    ResourceManager::loadShader("res/shaders/default2D_VS.glsl",
                                "res/shaders/debug_depth_FS.glsl",
                                nullptr, "debug_depth",
                                args,1);
    debugDepthShader = ResourceManager::getShader("debug_depth");

    this->skybox = Ref<CubeMap>(new CubeMap());
    std::vector<std::string> skyboxFaces
            {
                    "res/textures/skybox/right.jpg",
                    "res/textures/skybox/left.jpg",
                    "res/textures/skybox/top.jpg",
                    "res/textures/skybox/bottom.jpg",
                    "res/textures/skybox/front.jpg",
                    "res/textures/skybox/back.jpg"
            };
    this->skybox->loadCubeMap(skyboxFaces);
    renderer3D->setSkybox(skybox._get_ptr());

    ResourceManager::loadModel("res/models/rock_0.obj", "rock_0");
    ResourceManager::loadModel("res/models/rock_1.obj", "rock_1");
    ResourceManager::loadModel("res/models/rock_2.obj", "rock_2");

    ResourceManager::loadModel("res/models/shrub_0.obj", "shrub_0");
    ResourceManager::loadModel("res/models/shrub_1.obj", "shrub_1");
    ResourceManager::loadModel("res/models/shrub_2.obj", "shrub_2");

    ResourceManager::loadModel("res/models/flower_blue.obj", "flower_0");
    ResourceManager::loadModel("res/models/flower_red.obj", "flower_1");
    ResourceManager::loadModel("res/models/flower_yellow.obj", "flower_2");

    LoaderSettings::seperateMaterials = true;
    ResourceManager::loadModel("res/models/tree_0.obj", "tree_0");
    ResourceManager::loadModel("res/models/tree_1.obj", "tree_1");
    ResourceManager::loadModel("res/models/tree_2.obj", "tree_2");
    LoaderSettings::seperateMaterials = false;

    auto model = ResourceManager::getModel("tree_0");

    activeSeed = new uint32_t;
    *activeSeed = 8462;
    *activeSeed = 6051;
    *activeSeed = 513;
    tempSeed = new uint32_t;
    *tempSeed = *activeSeed;

    terrain = new Terrain();
    activeBiome = new Biome();
    //float terrainDivisor = 2.2f;
    float terrainDivisor = 2.0f;
    //activeBiome->colours.emplace_back(Color::create(201 / terrainDivisor, 178 / terrainDivisor, 99 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(201 / terrainDivisor, 178 / terrainDivisor, 99 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(201 / terrainDivisor, 178 / terrainDivisor, 99 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(201 / terrainDivisor, 178 / terrainDivisor, 99 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(135 / terrainDivisor, 184 / terrainDivisor, 82 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(135 / terrainDivisor, 184 / terrainDivisor, 82 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(80 / terrainDivisor, 171 / terrainDivisor, 93 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(80 / terrainDivisor, 171 / terrainDivisor, 93 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(120 / terrainDivisor, 120 / terrainDivisor, 120 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(120 / terrainDivisor, 120 / terrainDivisor, 120 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(200 / terrainDivisor, 200 / terrainDivisor, 210 / terrainDivisor));
    activeBiome->colours.emplace_back(Color::create(200 / terrainDivisor, 200 / terrainDivisor, 210 / terrainDivisor));
    terrain->generate(75, 75, 2, 2, *activeSeed, 1, *activeBiome);
    tempBiome = new Biome(*activeBiome);

    renderer3D->setTerrain(terrain);

    Input::init();
    // setting up input callbacks
    glfwSetKeyCallback(window, Input::KeyCallback);
    glfwSetCursorPosCallback(window, Input::CursorPositionCallback);
    glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
    glfwSetScrollCallback(window, Input::ScrollCallback);
    glfwSetWindowSizeCallback(window, Input::WindowSizeCallback);

    this->mcc = new MouseCameraController(this->camera);
    this->mcc->setup();

    orthoCamera = new OrthographicCamera(0, 1280, 0, 720);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    testFrameBuffer = new FrameBuffer(1280, 720);
    testFrameBuffer->createColourAttachment();

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
        orthoCamera->resize(display_w, display_h);
        //camera->setPosition(glm::vec3(0, 0, -3));
        camera->update();
        orthoCamera->update();
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
            terrain->generate(75, 75, 2, 2, *activeSeed, 1, *activeBiome);
            tempBiome = new Biome(*activeBiome);
        }

        auto start = chrono_now();
        renderer3D->begin();
        renderer3D->end();
        auto end = chrono_now();
        //std::cout<<terrain->getHeight(0,0)<<std::endl;

        std::chrono::duration<float> duration = end - start;
        float fDuration = duration.count() * 1000.0f; // to ms

        renderer2D->setProjectionMatrix(orthoCamera->getCombined());

        renderer2D->begin();
        renderer2D->draw(*renderer3D->getReflectionFb().colourAttachment, glm::vec2(0, 0), glm::vec2(128, 72),
                         0, 0, 2, 2);
        renderer2D->draw(*renderer3D->getRefractionFb().colourAttachment, glm::vec2(128 * 2, 0), glm::vec2(128, 72),
                         0, 0, 2, 2);
        renderer2D->end();


        startTime = glfwGetTime() - startTime;
        startTime *= 1000;


        ImGui::Begin("Terrain info");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        ImGui::Text("Game loop: %.3f ms.", deltaTime *= 1000);
        ImGui::Text("Drawing terrain: %.3f ms.", fDuration);
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
    delete skybox._get_ptr();
    ResourceManager::dispose();

}
