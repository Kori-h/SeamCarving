#include "pch.h"
#include "glapp.hpp"

GLApp::GLApp(int width, int height, std::string const& name)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return;
    }

    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
    }
}

void GLApp::Shutdown()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool GLApp::IsRunning()
{
    return !glfwWindowShouldClose(window);
}

void GLApp::Poll()
{
    glfwPollEvents();
}

void GLApp::Render()
{
    glfwSwapBuffers(window);
}