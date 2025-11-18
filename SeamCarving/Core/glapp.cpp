#include "../pch.h"
#include "glapp.hpp"

GLApp::GLApp(int width, int height, std::string const& name)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialise GLFW!" << std::endl;
        return;
    }
    std::cout << "GLFW initialised." << std::endl;

    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return;
    }
    std::cout << "GLFW window created." << std::endl;

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialise GLEW!" << std::endl;
    }
    std::cout << "GLEW initialised" << std::endl;

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);    
}

void GLApp::Shutdown()
{
    if (window) 
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

bool GLApp::IsRunning()
{
    return window && !glfwWindowShouldClose(window);
}

void GLApp::ClearFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLApp::Poll()
{
    glfwPollEvents();
}

void GLApp::Render()
{
    glfwSwapBuffers(window);
}