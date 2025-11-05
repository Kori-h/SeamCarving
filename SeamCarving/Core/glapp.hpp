#pragma once

struct GLApp
{
    GLApp(int width, int height, std::string const& name);
    void Shutdown();
    bool IsRunning();
    void ClearFrame();
    void Poll();
    void Render();;

    GLFWwindow* window;
};