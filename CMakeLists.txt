# @formatter:off
cmake_minimum_required(VERSION 3.17)
project(TowerDefense)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_BUILD_TYPE RELEASE)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

file(GLOB_RECURSE libs
        "external/glad/src/glad.c"
        "external/imgui/*.cpp"
        "external/stb_image/*.cpp"
        "external/stb_truetype/*.cpp"
        )

file(GLOB_RECURSE src
        "src/*.cpp"
        "src/*.h"
        "src/*.hpp"
        )

find_package(OpenGL REQUIRED)
if (NOT OPENGL_FOUND)
    message("ERROR: OpenGL not found")
endif ()

add_subdirectory(external/glfw-3.3.2)

include_directories(external/glad/include/)
include_directories(external/imgui/)
include_directories(external/stb_image/)
include_directories(external/stb_truetype/)
include_directories(external/stb_vorbis/)
include_directories(external/glm/)
include_directories(external/openal-soft/include)

set(SOURCE src/main.cpp)
set(PROJECT_NAME TowerDefense)
set(EXTERNAL
    ${OPENGL_LIBRARY}
    glfw
    )

# GLFW library
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_executable(${PROJECT_NAME} ${SOURCE} ${src} ${libs})

target_link_libraries(${PROJECT_NAME} ${EXTERNAL})
