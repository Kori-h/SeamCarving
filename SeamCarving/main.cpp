#include "pch.h"
#include "stbloader.hpp"
#include "glapp.hpp"
#include "gui.hpp"

int main() 
{    
    GLApp app(1280, 720, "App");
    GUI gui(app.window);
  
    GLuint textureID = LoadTexture("your_image.png");

    while (app.IsRunning())
    {
        app.Poll();
        gui.NewFrame();

        ImGui::Begin("Texture and Buttons");
        {
            ImGui::Text("Displaying Texture:");
            ImVec2 image_size(300, 300);
            ImGui::Image((void*)(intptr_t)textureID, image_size);

            if (ImGui::Button("Button 1")) std::cout << "Button 1 clicked!" << std::endl;
            if (ImGui::Button("Button 2")) std::cout << "Button 2 clicked!" << std::endl;
        }
        ImGui::End();

        gui.EndFrame();
        app.Render();
    }

    gui.Shutdown();
    app.Shutdown();
    return 0;
}