//
// Created by klemen on 12/12/2020.
//

#include "../engine/Core.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <sstream>
#include <utility>
#include "Game.h"
#include "../engine/ResourceManager.h"
#include "../engine/rendering/RenderingCapabilities.h"
#include "../engine/loader/Loader.h"

Screen *currentScreenStatic;
std::string gameTitle;

Game::Game() : window(nullptr)
{
    gameTitle = "";
}


/*
 * Destructor
 */
Game::~Game()
{
    // cleaning up
    for (auto &itr : screenMap)
        delete itr.second;

    screenMap.clear();

    delete renderer2D;
    delete renderer3D;

    ResourceManager::dispose();
    Input::dispose();

    glfwTerminate();
}

/*
 * Initialized OpenGL context and ImGui
 */
bool Game::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    width = 1280;
    height = 720;

    window = glfwCreateWindow(width, height, gameTitle.c_str(), nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // syncs to monitor refresh rate
    auto *videomode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, (videomode->width - width) / 2, (videomode->height - height) / 2);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glViewport(0, 0, width, height);
    std::cout << glGetString(GL_VERSION) << std::endl;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char *glsl_version = "#version 450";
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void Game::start()
{
    RenderingCapabilities::init();
    ResourceManager::init();

    // todo: loading assets asynchronous
    ResourceManager::loadWhitePixelTexture();

    ShaderSourceArgument args[1];
    args[0] = ShaderSourceArgument{FRAGMENT, "TEXTURE_SLOTS",
                                   std::to_string(RenderingCapabilities::MAX_TEXTURE_IMAGE_UNITS)};
    ResourceManager::loadShader("res/shaders/default2D_VS.glsl",
                                "res/shaders/default2D_FS.glsl",
                                nullptr, "default2D",
                                args, 1);

    ResourceManager::loadFont("res/fonts/Roboto-Black.ttf", "roboto");

    // loading models
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

    // skybox
    ResourceManager::loadCubeMap("res/textures/skybox","jpg","skybox");
    // biome
    Biome *terrainBiome = new Biome();
    float d = 2.0f;
    terrainBiome->colours.emplace_back(Color::create(201 / d, 178 / d, 99 / d));
    terrainBiome->colours.emplace_back(Color::create(201 / d, 178 / d, 99 / d));
    terrainBiome->colours.emplace_back(Color::create(201 / d, 178 / d, 99 / d));
    terrainBiome->colours.emplace_back(Color::create(135 / d, 184 / d, 82 / d));
    terrainBiome->colours.emplace_back(Color::create(135 / d, 184 / d, 82 / d));
    terrainBiome->colours.emplace_back(Color::create(80 / d, 171 / d, 93 / d));
    terrainBiome->colours.emplace_back(Color::create(80 / d, 171 / d, 93 / d));
    terrainBiome->colours.emplace_back(Color::create(120 / d, 120 / d, 120 / d));
    terrainBiome->colours.emplace_back(Color::create(120 / d, 120 / d, 120 / d));
    terrainBiome->colours.emplace_back(Color::create(200 / d, 200 / d, 210 / d));
    terrainBiome->colours.emplace_back(Color::create(200 / d, 200 / d, 210 / d));
    ResourceManager::loadBiome(terrainBiome, "defaultBiome");


    renderer2D = new Renderer2D(ResourceManager::getShader("default2D"));
    renderer3D = new Renderer3D();
    Input::init();

    // setting up input callbacks
    glfwSetKeyCallback(window, Input::KeyCallback);
    glfwSetCursorPosCallback(window, Input::CursorPositionCallback);
    glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
    glfwSetScrollCallback(window, Input::ScrollCallback);
    glfwSetWindowSizeCallback(window, Input::WindowSizeCallback);


    initScreens();
    for (auto &it:screenMap)
        it.second->init();

    //setScreen(TEST);
    setScreen(MAIN_MENU_SCREEN);
}

std::stringstream ss;

/*
 * Main game loop
 */
void Game::run()
{
    int frames = 0;
    double time = 0;
    double lastTime;
    double currentTime = 0;

    /* Main game loop with
     *
     * https://gameprogrammingpatterns.com/game-loop.html
     */
    do
    {
        // update
        lastTime = currentTime;
        elapsedTime = (float)glfwGetTime();
        currentTime = glfwGetTime();
        frames++;
        auto dt = (float) (currentTime - lastTime);

        if (currentTime - time > 1.0f)
        {
            //std::cout << "FPS: " << frames << std::endl;
            //ss << gameTitle << " ["<<(1/dt)<<" FPS]";
            ss.str(std::string()); // clearing ss
            ss << gameTitle << " [" << frames << " FPS]";
            glfwSetWindowTitle(window, ss.str().c_str());

            time = currentTime;
            frames = 0;
        }

        /* Poll for and process events */
        glfwPollEvents();
        Input::handleEvents();

        // update
        currentScreen->update(dt);
        // render
        currentScreen->render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

    } while (!glfwWindowShouldClose(window) && isRunning);
}

/*
 * Switches screen
 */
void Game::setScreen(ScreenType screenType)
{
    if (currentScreen)
        currentScreen->close();

    auto &newScreen = screenMap[screenType];
    if (!newScreen)
    {
        std::cout << "[Error] Screen does not exist!" << std::endl;
        std::exit(-1);
    }

    currentScreen = newScreen;
    currentScreen->show();
}

void Game::setTitle(std::string newTitle)
{
    gameTitle = std::move(newTitle);
    glfwSetWindowTitle(window, ss.str().c_str());
}

void Game::setIsRunning(bool _isRunning)
{
    Game::isRunning = _isRunning;
}
