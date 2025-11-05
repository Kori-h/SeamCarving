#include "pch.h"

// core libs
#include "Core/stbloader.hpp"
#include "Core/glapp.hpp"
#include "Core/gui.hpp"

// seam carving using dynamic programming
#include "SeamCarving/seamcarvingdp.hpp"

int main() 
{
    GLApp app(1280, 720, "Seam Carving Demo (Algorithm Analysis)");
    GUI gui(app.window);
    
    static char texturePath[256] = "Assets/sample.jpg";
    Texture texture = LoadTexture(texturePath);
    UpdateTexture(texture);

    static bool isProcessing = false;

    while (app.IsRunning())
    {
        app.ClearFrame();
        app.Poll();
        gui.NewFrame();

        ImGui::BeginDisabled(isProcessing);
        ImGui::Begin("Image", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
        {
            static float zoom = 1.0f;
            static const float zoomStep = 0.05f;

            if (ImGui::Button("Zoom In"))  zoom += zoomStep;
            ImGui::SameLine();
            if (ImGui::Button("Zoom Out")) zoom = std::max(0.1f, zoom - zoomStep);
            ImGui::Text("Zoom: %.0f%%", zoom * 100.0f);

            ImGui::Image((void*)(intptr_t)texture.id, ImVec2(texture.width * zoom, texture.height * zoom));
        }
        ImGui::End();

        ImGui::Begin("Image Controls");
        {
            ImGui::InputText("Texture Path", texturePath, IM_ARRAYSIZE(texturePath));

            if (ImGui::Button("Reload Image"))
            {
                texture = LoadTexture(texturePath);
                UpdateTexture(texture);
            }
        }
        ImGui::End();

        ImGui::Begin("Basic Controls");
        {
            if (ImGui::Button("Remove Horizontal (DP)"))
            {
                Grid<float> energy = DP::ComputeEnergy(texture);
                std::vector<int> seam = DP::FindHorizontalSeam(energy);
                DP::RemoveHorizontalSeam(texture, seam);
                UpdateTexture(texture);
            }

            if (ImGui::Button("Remove Vertical (DP)")) 
            {
                Grid<float> energy = DP::ComputeEnergy(texture);
                std::vector<int> seam = DP::FindVerticalSeam(energy);
                DP::RemoveVerticalSeam(texture, seam);
                UpdateTexture(texture);
            }

            if (ImGui::Button("Remove Lowest Energy Seam (DP)"))
            {
                Grid<float> energy = DP::ComputeEnergy(texture);
                auto vSeam = DP::FindVerticalSeam(energy);
                auto hSeam = DP::FindHorizontalSeam(energy);
                float vEnergy = DP::CalculateVerticalSeamEnergy(energy, vSeam);
                float hEnergy = DP::CalculateHorizontalSeamEnergy(energy, hSeam);
                if (vEnergy < hEnergy) DP::RemoveVerticalSeam(texture, vSeam);
                else DP::RemoveHorizontalSeam(texture, hSeam);
                UpdateTexture(texture);
            } 
        }
        ImGui::End();
      
        ImGui::Begin("Resize Controls (DP)");
        {
            static bool isResizing = false;
            static int targetWidth = texture.width;
            static int targetHeight = texture.height;

            ImGui::InputInt("Target Width", &targetWidth);
            ImGui::InputInt("Target Height", &targetHeight);

            targetWidth = std::clamp(targetWidth, 1, texture.width);
            targetHeight = std::clamp(targetHeight, 1, texture.height);

            if (ImGui::Button("Resize Image"))
            {
                isResizing = true;
                isProcessing = true; 
            }

            if (isResizing)
            {
                if (texture.width > targetWidth || texture.height > targetHeight)
                {
                    Grid<float> energy = DP::ComputeEnergy(texture);
                    std::vector<int> vSeam;
                    std::vector<int> hSeam;
                    float vEnergy = std::numeric_limits<float>::max();
                    float hEnergy = std::numeric_limits<float>::max();

                    if (texture.width > targetWidth)
                    {
                        vSeam = DP::FindVerticalSeam(energy);
                        vEnergy = DP::CalculateVerticalSeamEnergy(energy, vSeam);
                    }

                    if (texture.height > targetHeight)
                    {
                        hSeam = DP::FindHorizontalSeam(energy);
                        hEnergy = DP::CalculateHorizontalSeamEnergy(energy, hSeam);
                    }

                    if (vEnergy < hEnergy) DP::RemoveVerticalSeam(texture, vSeam);
                    else DP::RemoveHorizontalSeam(texture, hSeam);
                    UpdateTexture(texture);
                }
                else
                {
                    std::cout << "Resizing completed. Final size: " << texture.width << "x" << texture.height << std::endl;
                    isProcessing = false;
                    isResizing = false;
                }            
            }
        }
        ImGui::End();
        ImGui::EndDisabled();

        gui.EndFrame();
        app.Render();
    }
    
    // clean up
    gui.Shutdown();
    app.Shutdown();
    return 0;
}