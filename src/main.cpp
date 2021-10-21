#include <glad/glad.h>
#include <sstream>

#include "game/Game.h"
#include "engine/Core.h"

#include "tests/BatchingTest.h"
#include "tests/TerrainTest.h"


#define RUN_BATCHING_TEST 0
#define TERRAIN_TEST 1

#define LOG_OPENGL_NOTIFICATIONS 1

// Callback function for printing debug statements
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar* msg, const void* data);


int main(int argc, char** argv)
{

#if RUN_BATCHING_TEST
    BatchingTest batchingTest{};
    batchingTest.initTest();
    batchingTest.init();
    batchingTest.start();
    return 0;
#elif TERRAIN_TEST
    TerrainTest terrainTest{};
    terrainTest.initTest();
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLDebugMessageCallback, nullptr);
    terrainTest.init();
    terrainTest.start();
    return 0;
#else
    // declaring game on the free store (heap memory)
    Game game{};
    game.init();

#if DEBUG_MODE
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLDebugMessageCallback, nullptr);
#endif
    game.start();
    game.setTitle("IslandDefense");
    // runs the game
    game.run();

    return 0;
#endif
}

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar* msg, const void* data)
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
            _source = "OTHER";
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
#if not LOG_OPENGL_NOTIFICATIONS
            return;
#else
            _severity = "NOTIFICATION";
            break;
#endif
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