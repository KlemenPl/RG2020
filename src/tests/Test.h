//
// Created by klemen on 12/12/2020.
//

#ifndef TOWERDEFENSE_TEST_H
#define TOWERDEFENSE_TEST_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "BunnyMarkTest.h"
#include "Test.h"
#include "../engine/ResourceManager.h"
#include "../engine/rendering/RenderingCapabilities.h"

/*
 *************
 * INTERFACE
 *************
 */
class Test
{
protected:
    GLFWwindow * window{};
public:

    void initTest()
    {
        // creating opengl context
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif
        glfwWindowHint(GLFW_RESIZABLE, false);

        this->window = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return ;
        }

        // intializing Dear_ImGui

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        const char *glsl_version = "#version 450";
        ImGui_ImplOpenGL3_Init(glsl_version);

        glViewport(0, 0, 1280, 720);

        RenderingCapabilities::init();
        ResourceManager::init();
    }

    // called to initialize the test
    virtual void init() = 0;

    // called each frame
    virtual void render() = 0;

};

#endif //TOWERDEFENSE_TEST_H
