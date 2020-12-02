# @formatter:off
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

include_directories(external/glew-2.1.0/
                    external/glew-2.1.0/include/
                    external/glfw-3.3.2/
                    external/glfw-3.3.2/include/
                    )

find_package(OpenGL REQUIRED)
if (NOT OPENGL_FOUND)
    message("ERROR: OpenGL not found")
endif ()

set(SOURCE src/main.cpp)
set(PROJECT_NAME TowerDefense)
set(EXTERNAL
    ${OPENGL_LIBRARY}
    glfw
    GLEW)

add_executable(${PROJECT_NAME} ${SOURCE} ${src} ${libs})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

target_link_libraries(${PROJECT_NAME} ${EXTERNAL})

# GLFW libraries
##add_subdirectory(libs/glfw)
##set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
##set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
##set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
##TARGET_LINK_LIBRARIES(${PROJECT_NAME} glfw)
