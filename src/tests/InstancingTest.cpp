//
// Created by klemen on 18/12/2020.
//

#include <functional>
#include "InstancingTest.h"
#include "../engine/loader/Loader.h"
#include "../engine/input/Input.h"

void InstancingTest::init()
{
    ResourceManager::loadWhitePixelTexture();
    ResourceManager::loadShader("res/shaders/bunny_VS.glsl",
                                "res/shaders/bunny_FS.glsl",
                                nullptr, "bunny");
    ResourceManager::loadShader("res/shaders/light_VS.glsl",
                                "res/shaders/light_FS.glsl",
                                nullptr, "light");
    this->shader = ResourceManager::getShader("bunny");
    this->lightShader = ResourceManager::getShader("light");

    ResourceManager::loadFont("res/fonts/Roboto-Black.ttf", "roboto");
    this->font = ResourceManager::getFont("roboto");
    bunny = Ref<RawModel>(Loader::loadOBJ("res/models/bunny.obj"));
    //bunny = Ref<RawModel>(Loader::loadOBJ("res/models/turret_single.obj"));
    bunny->generateMeshes();

    cube = Ref<RawModel>(Loader::loadOBJ("res/models/cube.obj"));
    cube->generateMeshes();

    this->camera = new PerspectiveCamera(1280, 720);

    this->lightPos = glm::vec3(2, 1, 2);

    this->lightMaterial.Ka = glm::vec3(1.0f, 1.0f, 1.0f);
    this->lightMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    this->lightMaterial.Ks = glm::vec3(1.0f, 1.0f, 1.0f);

    // http://devernay.free.fr/cours/opengl/materials.html
    // gold
    this->material.Ka = glm::vec3(0.24725f, 0.1995f, 0.0745f);
    this->material.Kd = glm::vec3(0.75164f, 0.60648f, 0.22648f);
    this->material.Ks = glm::vec3(0.628281f, 0.555802f, 0.366065f);
    //this->material.Ns = 0.4f * 128.0f;
    this->material.Ns = 0.4f * 10;

    // silver
    // this->material.Ka = glm::vec3(0.19225f, 0.19225f, 0.19225f);
    // this->material.Kd = glm::vec3(0.50754f, 0.50754f, 0.50754f);
    // this->material.Ks = glm::vec3(0.508273f, 0.508273f, 0.508273f);
    // this->material.Ns = 0.4f * 128.0f;

    //this->material.Ka = glm::vec3(0.000000, 0.000000, 0.000000);
    //this->material.Kd = glm::vec3(0.274510, 0.298039, 0.341176);
    //this->material.Ks = glm::vec3(0.500000, 0.500000, 0.500000);
    //this->material.Ns = 225.0f;

    Input::init();
    // setting up input callbacks
    glfwSetKeyCallback(window, Input::KeyCallback);
    glfwSetCursorPosCallback(window, Input::CursorPositionCallback);
    glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
    glfwSetScrollCallback(window, Input::ScrollCallback);
    glfwSetWindowSizeCallback(window, Input::WindowSizeCallback);

    this->cameraController = new MouseCameraController(this->camera);
    this->cameraController->setup();

    std::function<bool()> f = [this]() -> bool {
        std::cout << "F key pressed " << this->material.Ns << std::endl;
        return true;
    };
    Input::addKeyEvent(KeyEvent(KEY_F, PRESS, f));
    //auto f1 = []() -> bool { return true; };
    //void (*)(bool) f = []() -> bool { return true; };

    modelMatrices = new glm::mat4[maxBunnies];
    glm::vec3 offset = glm::vec3(0, 0, 0);
    float offsetStep = 0.5f;
    int numInRow = 20;
    for (uint32_t i = 0; i < maxBunnies; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::translate(model, offset);
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

        offset.z += offsetStep;

        if (i > 0 && (i % numInRow) == 0)
        {
            offset.x += offsetStep;
            offset.y = 0;
            offset.z = 0;
        }


        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    // configure instanced array
    // -------------------------
    unsigned int VAO = bunny->groups.front().mesh.VAO;
    glBindVertexArray(VAO);
    glGenBuffers(1, &instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxBunnies * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    // set attribute pointers for matrix (4 times vec4)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) 0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

}
void InstancingTest::start()
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

        float startTime=glfwGetTime();

        shader->bind();
        const Mesh &mesh = bunny->groups.front().mesh;
        mesh.bind();
        glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

        // a bit reduntant
        shader->setUniformMaterial("material", material);
        shader->setUniform("combined", camera->getCombined());

        shader->setUniform("light.pos", lightPos);
        shader->setUniformMaterial2("light", lightMaterial);


        // drawing bunnies
        //glBindVertexArray(mesh.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, mesh.indicesLength, GL_UNSIGNED_INT, 0, numberOfBunnies);
        //glBindVertexArray(0);



        // drawing light
        lightShader->bind();
        lightShader->setUniform("colour", lightMaterial.Ka);
        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        lightShader->setUniform("model", model, false);
        lightShader->setUniform("combined", camera->getCombined(),false);
        const Mesh &cubeMesh = cube->groups.front().mesh;

        cubeMesh.bind();
        glDrawElements(GL_TRIANGLES, cubeMesh.indicesLength, GL_UNSIGNED_INT, nullptr);

        startTime = glfwGetTime() - startTime;
        startTime*=1000;


        ImGui::Begin("Render info");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        ImGui::Text("Game loop: %.3f ms.", deltaTime *= 1000);
        ImGui::Text("Only render loop: %.3f ms.", startTime);

        ImGui::Text("OpenGL draw calls: %d.", 1);
        ImGui::Text("VBO buffer size: %.2f kB.", (mesh.verticesLength * sizeof(float)+
                                               numberOfBunnies*16*sizeof (float ))/1000.0f);
        ImGui::Text("IBO buffer size: %.2f kB.", (mesh.indicesLength * sizeof(uint32_t))/1000.0f);
        ImGui::Text("Number of objects: %d", numberOfBunnies);
        ImGui::SliderInt("", &numberOfBunnies, 0, maxBunnies);
        ImGui::SliderFloat3("Light position", &lightPos[0], -20, 20);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    delete camera;
    delete bunny.operator->();
    delete cube.operator->();

    ResourceManager::dispose();

}
