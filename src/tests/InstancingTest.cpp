//
// Created by klemen on 18/12/2020.
//

#include "InstancingTest.h"
#include "../engine/loader/Loader.h"

void InstancingTest::init()
{
    ResourceManager::loadWhitePixelTexture();
    ResourceManager::loadShader("res/shaders/bunny_VS.glsl",
                                "res/shaders/bunny_FS.glsl",
                                nullptr, "bunny");
    this->shader = ResourceManager::getShader("bunny");

    ResourceManager::loadFont("res/fonts/Roboto-Black.ttf", "roboto");
    this->font = ResourceManager::getFont("roboto");

    //bunny = Ref<RawModel>(Loader::loadOBJ("res/models/bunny.obj"));
    bunny = Ref<RawModel>(Loader::loadOBJ("res/models/bunny.obj"));
    bunny->generateMeshes();

    this->camera = new PerspectiveCamera(1280, 720);

    this->lightPos = glm::vec3(10, 0, 3);

    this->lightMaterial.Ka = glm::vec3(1.0f, 1.0f, 1.0f);
    this->lightMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    this->lightMaterial.Ks = glm::vec3(1.0f, 1.0f, 1.0f);

    // http://devernay.free.fr/cours/opengl/materials.html
    // gold
    // this->material.Ka = glm::vec3(0.24725f, 0.1995f, 0.0745f);
    // this->material.Kd = glm::vec3(0.75164f, 0.60648f, 0.22648f);
    // this->material.Ks = glm::vec3(0.628281f, 0.555802f, 0.366065f);
    // this->material.Ns = 0.4f * 128.0f;

    // silver
    this->material.Ka = glm::vec3(0.19225f, 0.19225f, 0.19225f);
    this->material.Kd = glm::vec3(0.50754f, 0.50754f, 0.50754f);
    this->material.Ks = glm::vec3(0.508273f, 0.508273f, 0.508273f);
    this->material.Ns = 0.4f * 128.0f;


    glEnable(GL_DEPTH_TEST);

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

        shader->bind();
        for (const auto &group:bunny->groups)
        {
            for (const auto &mesh:group.meshes)
            {
                mesh.bind();
                shader->setUniformMaterial("material", mesh.defaultMaterial);
                shader->setUniformMaterial("light", Material());
                shader->setUniform("combined", camera->getCombined());

                shader->setUniform("light.pos", lightPos);
                shader->setUniformMaterial2("light", lightMaterial);

                shader->setUniformMaterial("material", material);

                glm::mat4 model = glm::mat4(1.0f);
                //model = glm::translate(model, glm::vec3(0,0,0));
                //model = glm::rotate(model, (float) glfwGetTime(), glm::vec3(0, 1, 0));
                model = glm::scale(model, glm::vec3(0.5f));
                shader->setUniform("model", model);

                glDrawElements(GL_TRIANGLES, mesh.indicesLength, GL_UNSIGNED_INT, nullptr);

            }
        }


        ImGui::Begin("Render info");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    delete camera;

    ResourceManager::dispose();

}
