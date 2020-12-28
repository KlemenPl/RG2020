//
// Created by klemen on 23/12/2020.
//

#include "Instancing2Test.h"

#include "../engine/loader/Loader.h"
#include "../engine/input/Input.h"

Model *treeModel;

void Instancing2Test::init()
{
    ResourceManager::loadWhitePixelTexture();
    ResourceManager::loadShader("res/shaders/bunny_VS.glsl",
                                "res/shaders/bunny_FS.glsl",
                                nullptr, "bunny");
    ResourceManager::loadShader("res/shaders/light_VS.glsl",
                                "res/shaders/light_FS.glsl",
                                nullptr, "light");
    ResourceManager::loadFont("res/fonts/Roboto-Black.ttf", "roboto");
    this->font = ResourceManager::getFont("roboto");
    this->renderer3D = new Renderer3D;

    //tree = Ref<RawModel>(Loader::loadOBJ("res/models/tree_01.obj"));
    tree = Ref<RawModel>(Loader::loadOBJ("res/models/turret_single.obj", false));
    tree->generateMeshes();
    treeModel = new Model(*tree);

    this->renderer3D->addDirLight(DirLight(glm::vec3(1, -0.21f, -0.2)));
    this->renderer3D->addPointLight(PointLight(glm::vec3(2, 2, 2)));

    this->camera = new PerspectiveCamera(1280, 720);

    Input::init();
    // setting up input callbacks
    glfwSetKeyCallback(window, Input::KeyCallback);
    glfwSetCursorPosCallback(window, Input::CursorPositionCallback);
    glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
    glfwSetScrollCallback(window, Input::ScrollCallback);
    glfwSetWindowSizeCallback(window, Input::WindowSizeCallback);

    this->cameraController = new MouseCameraController(this->camera);
    this->cameraController->setup();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glm::perlin(glm::vec2(1,1));

    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            std::cout<<glm::perlin(glm::vec2(i/20.0f,j/20.0f))<<"\t";
        }
        std::cout<<std::endl;
    }

}
void Instancing2Test::start()
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

        renderer3D->setCamera(camera);
        renderer3D->begin();
        Model m1 = treeModel->clone();
        m1.modelGroups.front().position.x = -0.5f;
        renderer3D->draw(&m1);
        renderer3D->draw(treeModel);
        renderer3D->end();

        //renderer3D->drawNormals(treeModel);

        //treeModel->rawModel.groups.front().mesh.bind();
        //glDrawElements(GL_TRIANGLES, treeModel->rawModel.groups.front().mesh.indicesLength, GL_UNSIGNED_INT, nullptr);


        /* drawing light
        lightShader->bind();
        lightShader->setUniform("colour", lightMaterial.Ka);
        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        lightShader->setUniform("model", model, false);
        lightShader->setUniform("combined", camera->getCombined(), false);
        const Mesh &cubeMesh = cube->groups.front().mesh;

        cubeMesh.bind();
        glDrawElements(GL_TRIANGLES, cubeMesh.indicesLength, GL_UNSIGNED_INT, nullptr);
         */



        ImGui::Begin("Render info");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    delete camera;
    delete tree.operator->();
    delete renderer3D;
    delete treeModel;

    ResourceManager::dispose();

}