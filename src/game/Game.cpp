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
#include "Game.h"

Game::Game()
= default;

Game::~Game()
{
    // clean up
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

    window = glfwCreateWindow(1280, 720, "Game", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

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

}

/*
 * Main game loop
 */
void Game::run()
{

}

/*
 * switches screen
 */
void Game::setScreen(ScreenType screenType)
{

}