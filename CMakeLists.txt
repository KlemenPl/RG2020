# @formatter:off
cmake_minimum_required(VERSION 3.17)
project(TowerDefense)

set(CMAKE_CXX_STANDARD 14)

file(GLOB libs
        "external/glad/src/glad.c"
        "libs/imgui/*"
        "libs/stb_image/*"
        )

file(GLOB src
        "src/*"
        "src/engine/*"
        "src/engine/graphics/*"
        "src/engine/math/*"
        )

find_package(OpenGL REQUIRED)
if (NOT OPENGL_FOUND)
    message("ERROR: OpenGL not found")
endif ()

add_subdirectory(external/glfw-3.3.2)
include_directories(external/glad/include/
                    )

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
