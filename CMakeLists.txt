cmake_minimum_required(VERSION 3.17)
project(TowerDefense)

set(CMAKE_CXX_STANDARD 14)

file(GLOB libs
        "libs/imgui/*"
        "libs/stb_image/*"
        )

file(GLOB src
        "src/*"
        )

set(SOURCE src/main.cpp)
set(PROJECT_NAME TowerDefense)

add_executable(${PROJECT_NAME} ${SOURCE} ${src} ${libs})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FIND_PACKAGE(OpenGL REQUIRED)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} glfw)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} OpenGL::GL)

find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)


target_link_libraries(${PROJECT_NAME} assimp)