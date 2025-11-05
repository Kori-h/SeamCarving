#pragma once

struct GUI
{
    GUI(GLFWwindow* window);
    void Shutdown();
    void NewFrame();
    void EndFrame();
};