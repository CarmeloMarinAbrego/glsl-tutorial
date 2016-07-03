cmake_minimum_required(VERSION 3.4)

project(00-GLSL-Introduccion)

find_package(OpenGL REQUIRED)

include_directories(../extra/glfw/deps)                    
set(GLAD "../extra/glfw/deps/glad/glad.h"
         "../extra/glfw/deps/glad.c")
         
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME} glsl-intro.c ${GLAD})

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} ${OPENGL_gl_LIBRARY})