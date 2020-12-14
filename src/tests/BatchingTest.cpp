//
// Created by klemen on 12/12/2020.
//

#include "BatchingTest.h"
#include <string>

static int getRandom(int min, int max)
{
    return min + (std::rand() % (max - min + 1));
}

void BatchingTest::init()
{
    // loading shader and setting argument for TEXTURE_SLOTS
    ShaderSourceArgument args[1];
    args[0] = ShaderSourceArgument{FRAGMENT, "TEXTURE_SLOTS",
                                   std::to_string(RenderingCapabilities::MAX_TEXTURE_IMAGE_UNITS)};

    ResourceManager::loadWhitePixelTexture();

    ResourceManager::loadShader("res/shaders/default2D_VS.glsl",
                                "res/shaders/default2D_FS.glsl",
                                nullptr, "default2D",
                                args, 1);


    ResourceManager::loadFont("res/fonts/Roboto-Black.ttf", "roboto");
    this->font = &ResourceManager::getFont("roboto");

    std::string textures[30]{
            "bear",
            "buffalo",
            "chick",
            "chicken",
            "cow",
            "crocodile",
            "dog",
            "duck",
            "elephant",
            "frog",
            "giraffe",
            "goat",
            "gorilla",
            "hippo",
            "horse",
            "monkey",
            "moose",
            "narwhal",
            "owl",
            "panda",
            "parrot",
            "penguin",
            "pig",
            "rabbit",
            "rhino",
            "sloth",
            "snake",
            "walrus",
            "whale",
            "zebra"
    };

    for (auto &str:textures)
    {
        std::string path = "res/textures/" + str + ".png";
        ResourceManager::loadTexture(path.c_str(), str, true);
    }

    this->renderer2D = new Renderer2D(&ResourceManager::getShader("default2D"));
    this->camera = new OrthographicCamera(0, 1280, 0, 720);


    this->sprites=new std::vector<Sprite*>;

    for (int i = 0; i < maxSprites; i++)
    {
        Sprite *sprite = new Sprite(Vec2f(getRandom(0, 1280), getRandom(0, 720)),
                                          Vec2f(getRandom(-100, 100), getRandom(-100, 100)),
                                                Vec2f(20, 20),
                                                ResourceManager::getTexture2D(textures[getRandom(0, 29)]));
        sprites->push_back(sprite);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void BatchingTest::start()
{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

        camera->update();
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.25f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        float startTime = glfwGetTime();

        renderer2D->setProjectionMatrix(camera->getCombined());
        camera->setPosition(Vec3f(0, 0, 0));
        renderer2D->begin();
        for (int i = 0; i < numberOfSprites; i++)
        {
            auto &sprite = sprites->at(i);
            sprite->update(deltaTime);
            if (sprite->position.x < 0 || sprite->position.x > 1280)
                sprite->movement.x = -sprite->movement.x;
            if (sprite->position.y < 0 || sprite->position.y > 720)
                sprite->movement.y = -sprite->movement.y;

            renderer2D->draw(sprite->texture, sprite->position, sprite->size,
                             Vec2f(0, 0), Vec2f(1.0f, 1.0f), Colors::WHITE, 0);
        }

        ss << "Num Sprites: ";
        ss << numberOfSprites;
        // drawing text background
        renderer2D->draw(ResourceManager::getTexture2D("sshape"), Vec2f(0, 720 - 35),
                         Vec2f(ResourceManager::getFont("roboto").
                                 getTextWidth(ss.str(), 0.5f) + 10, 40),
                         Vec2f(0, 0), Vec2f(1, 1),
                         Color(0, 0, 0, 0.6f));

        renderer2D->draw(ResourceManager::getFont("roboto"), ss.str(),
                         Vec2f(0, 720 - 25), Vec2f(0.5f, 0.5f), Colors::RED);

        startTime = glfwGetTime() - startTime;
        startTime *= 1000;

        // clearing ss
        ss.str(std::string());

        renderer2D->end();

        ImGui::Begin("Render info");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        ImGui::Text("Game loop: %.3f ms.", deltaTime *= 1000);
        ImGui::Text("Only render loop: %.3f ms.", startTime);

        ImGui::Text("OpenGL draw calls: %d.", renderer2D->getDrawCalls());
        ImGui::Text("VBO buffer size: %lu B.", renderer2D->getIndicesSize() * sizeof(float));
        ImGui::Text("IBO buffer size: %lu B.", renderer2D->getVerticesSize() * sizeof(uint32_t));
        ImGui::Text("Number of objects: %d", numberOfSprites);
        ImGui::SliderInt("", &numberOfSprites, 0, maxSprites);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

    }

    glfwTerminate();
}

