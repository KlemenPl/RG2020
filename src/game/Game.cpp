//
// Created by klemen on 12/12/2020.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <raudio.h>
#include <iostream>
#include <sstream>
#include <utility>
#include "Game.h"
#include "../engine/ResourceManager.h"
#include "../engine/rendering/RenderingCapabilities.h"

Screen *currentScreenStatic;
std::string gameTitle;

int width;
int height;

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

    ResourceManager::dispose();
    Input::dispose();

    glfwTerminate();
    CloseAudioDevice();
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

    InitAudioDevice();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char *glsl_version = "#version 450";
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void Game::start()
{
    initScreens();
    setScreen(TEST);


    RenderingCapabilities::init();
    ResourceManager::init();

    // todo: loading assets asynchronous


    Input::init();

    // setting up input callbacks
    glfwSetKeyCallback(window, Input::KeyCallback);
    glfwSetCursorPosCallback(window, Input::CursorPositionCallback);
    glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
    glfwSetScrollCallback(window, Input::ScrollCallback);
    glfwSetWindowSizeCallback(window, Input::WindowSizeCallback);


    for (auto &it:screenMap)
        it.second->init();
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
    double currentTime;

    while (!glfwWindowShouldClose(window))
    {

        // update
        lastTime = currentTime;
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

        // update
        currentScreen->update(dt);
        // render
        currentScreen->render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

    }
}

/*
 * switches screen
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