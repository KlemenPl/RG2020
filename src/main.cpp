#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <sstream>

#include "engine/math/Vector.h"
#include "engine/math/Matrix.h"
#include "engine/ResourceManager.h"
#include "engine/graphics/Shader.h"
#include "engine/rendering/Renderer2D.h"
#include "engine/camera/OrthographicCamera.h"
#include "engine/rendering/Sprite.h"
#include "engine/rendering/RenderingCapabilities.h"
#include "tests/BatchingTest.h"

#define DEBUG_MODE 1

#define RUN_BATCHING_TEST 1

// Callback function for printing debug statements
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar *msg, const void *data);

static int getRandom(int min, int max)
{
    return min + (std::rand() % (max - min + 1));
}

/*
 * Todo: start using smart pointers, raw pointers are evil
 */
int main(int argc, char *argv[])
{
#if RUN_BATCHING_TEST
    BatchingTest batchingTest{};
    batchingTest.initTest();
    batchingTest.init();
    batchingTest.start();
    return 0;
#else
#endif
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    InitAudioDevice();

    Music music = LoadMusicStream("res/music/VirtualWorld.ogg");
    music.looping = false;

    PlayMusicStream(music);



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

    glDebugMessageCallback(GLDebugMessageCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    // OpenGL configuration
    // --------------------
    glViewport(0, 0, 1280, 720);

    // deltaTime variables
    // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;


    RenderingCapabilities::init();
    ResourceManager::init();

    ShaderSourceArgument args[1];
    args[0] = ShaderSourceArgument{FRAGMENT, "TEXTURE_SLOTS",
                                   std::to_string(RenderingCapabilities::MAX_TEXTURE_IMAGE_UNITS)};

    ResourceManager::loadShader("res/shaders/default2D_VS.glsl", "res/shaders/default2D_FS.glsl",
                                nullptr, "default2D",
                                args, 1);


    std::string textures[30]{
            "bear",
            //"wabbit_alpha",
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

    ResourceManager::loadFont("res/fonts/Apocalypse.ttf", "roboto");



    //ResourceManager::loadTexture("res/textures/rabbit.png", "rabbit", true);
    for (auto &str:textures)
    {
        std::string path = "res/textures/" + str + ".png";
        ResourceManager::loadTexture(path.c_str(), str, true);
    }

    Renderer2D renderer2D(&ResourceManager::getShader("default2D"));
    OrthographicCamera camera(0, 1280, 0, 720);

    int numberOfSprites = 10;
    std::vector<Sprite> sprites;
    for (int i = 0; i < 100000; i++)
    {
        sprites.emplace_back(Vec2f(getRandom(0, 1280), getRandom(0, 720)),
                             Vec2f(getRandom(-100, 100), getRandom(-100, 100)),
                             Vec2f(20, 25),
                //Vec2f(0,0),
                //Vec2f(512,512),
                             ResourceManager::getTexture2D(textures[getRandom(0, 29)]));
        //ResourceManager::getTexture2D("wabbit_alpha"));
        //fontTexture);
    }


    while (!glfwWindowShouldClose(window))
    {
        UpdateMusicStream(music);
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

        /*
        if (numberOfSprites != sprites.size())
        {
            for(int i = sprites.size();i<numberOfSprites;i++){
                sprites.emplace_back(Vec2f(getRandom(0, 1280), getRandom(0, 720)),
                                     Vec2f(getRandom(-100, 100), getRandom(-100, 100)),
                                     Vec2f(20, 25), ResourceManager::getTexture2D(textures[getRandom(0, 29)]));
            }

            for(int i=sprites.size();i>numberOfSprites;i--){
                sprites.pop_back();
            }
        }
         */

        //camera.resize(display_w,display_h);
        camera.update();
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.25f, 1);
        glClear(GL_COLOR_BUFFER_BIT);


        renderer2D.setProjectionMatrix(camera.getCombined());
        camera.setPosition(Vec3f(0, 0, 0));
        //renderer2D.setProjectionMatrix(math::ortho(0,display_w,0,display_h));
        //renderer2D.drawUnbatched(Texture2D(), Vec2f{0, 0}, Vec2f{100.0f, 100.0f});
        renderer2D.begin();
        //for (auto &sprite:sprites)
        //numberOfSprites=0;
        for (int i = 0; i < numberOfSprites; i++)
        {
            auto &sprite = sprites[i];
            sprite.update(deltaTime);
            if (sprite.position.x < 0 || sprite.position.x > 1280)
                sprite.movement.x = -sprite.movement.x;
            if (sprite.position.y < 0 || sprite.position.y > 720)
                sprite.movement.y = -sprite.movement.y;

            renderer2D.draw(sprite.texture, sprite.position, sprite.size,
                            Vec2f(0, 0), Vec2f(1.0f, 1.0f), Colors::WHITE, 0, false, false);
        }

        //renderer2D.draw(ResourceManager::getFont("roboto"), "the quick brown fox jumps over the lazy dog",
        //                Vec2f(200, 200), Vec2f(1.0f, 1.0f));

        renderer2D.end();

        ImGui::Begin("Render info");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::Text("OpenGL draw calls: %d.", renderer2D.getDrawCalls());
        ImGui::Text("VBO buffer size: %lu B.", renderer2D.getIndicesSize() * sizeof(float));
        ImGui::Text("IBO buffer size: %lu B.", renderer2D.getVerticesSize() * sizeof(uint32_t));
        ImGui::Text("Number of objects: %d", numberOfSprites);
        ImGui::SliderInt("", &numberOfSprites, 0, 100000);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

    }

    glfwTerminate();
    UnloadMusicStream(music);
    CloseAudioDevice();

    return 0;
}
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar *msg, const void *data)
{
    std::string _source;
    std::string _type;
    std::string _severity;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            _source = "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            _source = "WINDOW SYSTEM";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            _source = "SHADER COMPILER";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            _source = "THIRD PARTY";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            _source = "APPLICATION";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            _source = "UNKNOWN";
            break;
        default:
            _source = "UNKNOWN";
            break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            _type = "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            _type = "DEPRECATED BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            _type = "UDEFINED BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            _type = "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            _type = "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER:
            _type = "OTHER";
            break;
        case GL_DEBUG_TYPE_MARKER:
            _type = "MARKER";
            break;
        default:
            _type = "UNKNOWN";
            break;
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            _severity = "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            _severity = "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            _severity = "LOW";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            _severity = "NOTIFICATION";
            break;
        default:
            _severity = "UNKNOWN";
            break;
    }

    std::stringstream ss;
    ss << "0x" << std::hex << id;
    printf("[OpenGL] %s: %s of %s severity, raised from %s: %s\n",
           ss.str().c_str(), _type.c_str(), _severity.c_str(), _source.c_str(), msg);
    ss.clear();
}