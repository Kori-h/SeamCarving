#include "pch.h"

// core libs
#include "Core/stbloader.hpp"
#include "Core/glapp.hpp"
#include "Core/gui.hpp"

// seam carving using dynamic programming
#include "SeamCarving/seamcarvingdp.hpp"

// seam carving using greedy algorithm
#include "SeamCarving/seamcarvinggreedy.hpp"

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

        // ======================
        //      IMAGE WINDOW
        // ======================
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

        // ======================
        //      IMAGE CONTROLS
        // ======================
        ImGui::Begin("Image Controls");
        {
            ImGui::InputText("Texture Path", texturePath, IM_ARRAYSIZE(texturePath));

            if (ImGui::Button("Export Image"))
            {
                std::string exportedPath = texturePath;
                size_t divider = exportedPath.find_last_of('.');
                exportedPath.insert(divider, "_exported");
                SaveTextureAsPNG(texture, exportedPath);
            }

            if (ImGui::Button("Reload Image"))
            {
                texture = LoadTexture(texturePath);
                UpdateTexture(texture);
            }

            if (ImGui::Button("Load Default Image"))
            {
                char const* target = "Assets/sample.jpg";
                std::size_t length = std::strlen(target) + 1;
                std::copy(target, target + length, texturePath);
                texture = LoadTexture(texturePath);
                UpdateTexture(texture);
            }

            if (ImGui::Button("Load Default Image (Low Resolution)"))
            {
                char const* target = "Assets/lowres.jpg";
                std::size_t length = std::strlen(target) + 1;
                std::copy(target, target + length, texturePath);
                texture = LoadTexture(texturePath);
                UpdateTexture(texture);
            }
        }
        ImGui::End();

        // ======================
        //      BASIC CONTROLS
        // ======================
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

            if (ImGui::Button("Remove Horizontal (Greedy)"))
            {
                Grid<float> energy = DP::ComputeEnergy(texture); // same energy computation
                std::vector<int> seam = Greedy::FindHorizontalSeam_Greedy(energy);
                Greedy::RemoveHorizontalSeam(texture, seam);
                UpdateTexture(texture);
            }

            if (ImGui::Button("Remove Vertical (Greedy)"))
            {
                Grid<float> energy = DP::ComputeEnergy(texture);
                std::vector<int> seam = Greedy::FindVerticalSeam_Greedy(energy);
                Greedy::RemoveVerticalSeam(texture, seam);
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
      
        // ======================
        //      RESIZE CONTROLS
        // ======================
        ImGui::Begin("Resize Controls (DP)");
        {
            static bool isResizing = false;
            static int targetWidth = texture.width;
            static int targetHeight = texture.height;

            ImGui::InputInt("Target Width", &targetWidth);
            ImGui::InputInt("Target Height", &targetHeight);

            targetWidth = std::clamp(targetWidth, 1, texture.width);
            targetHeight = std::clamp(targetHeight, 1, texture.height);

            if (ImGui::Button("Resize Image (DP)"))
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

        ImGui::Begin("Resize Controls (Greedy)");
        {
            static bool isResizingGreedy = false;
            static int  targetWidthGreedy = texture.width;
            static int  targetHeightGreedy = texture.height;

            ImGui::InputInt("Target Width (Greedy)", &targetWidthGreedy);
            ImGui::InputInt("Target Height (Greedy)", &targetHeightGreedy);

            targetWidthGreedy = std::clamp(targetWidthGreedy, 1, texture.width);
            targetHeightGreedy = std::clamp(targetHeightGreedy, 1, texture.height);

            if (ImGui::Button("Resize Image (Greedy)"))
            {
                isResizingGreedy = true;
                isProcessing = true;
            }

            // remove several greedy seams per frame so it’s not painfully slow
            const int seamsPerFrameGreedy = 5;   // tweak this (1 = slow, 10+ = fast)

            if (isResizingGreedy)
            {
                int seamsDone = 0;

                while (seamsDone < seamsPerFrameGreedy &&
                    (texture.width > targetWidthGreedy ||
                        texture.height > targetHeightGreedy))
                {
                    Grid<float> energy = DP::ComputeEnergy(texture);

                    std::vector<int> vSeam;
                    std::vector<int> hSeam;
                    float vEnergy = std::numeric_limits<float>::max();
                    float hEnergy = std::numeric_limits<float>::max();

                    // Greedy vertical seam if we still need to shrink width
                    if (texture.width > targetWidthGreedy)
                    {
                        vSeam = Greedy::FindVerticalSeam_Greedy(energy);
                        vEnergy = DP::CalculateVerticalSeamEnergy(energy, vSeam);
                    }

                    // Greedy horizontal seam if we still need to shrink height
                    if (texture.height > targetHeightGreedy)
                    {
                        hSeam = Greedy::FindHorizontalSeam_Greedy(energy);
                        hEnergy = DP::CalculateHorizontalSeamEnergy(energy, hSeam);
                    }

                    if (vEnergy == std::numeric_limits<float>::max() &&
                        hEnergy == std::numeric_limits<float>::max())
                    {
                        break;
                    }

                    if (vEnergy < hEnergy)
                        Greedy::RemoveVerticalSeam(texture, vSeam);
                    else
                        Greedy::RemoveHorizontalSeam(texture, hSeam);

                    ++seamsDone;
                }

                if (seamsDone > 0)
                    UpdateTexture(texture); // updates image as we go

                if (texture.width <= targetWidthGreedy &&
                    texture.height <= targetHeightGreedy)
                {
                    std::cout << "Greedy resizing completed. Final size: "
                        << texture.width << "x" << texture.height << std::endl;
                    isProcessing = false;
                    isResizingGreedy = false;
                }
            }
        }
        ImGui::End();

        //ImGui::Begin("Resize Controls (Greedy)");
        //{
        //    static bool isResizingGreedy = false;
        //    static int targetWidthGreedy = texture.width;
        //    static int targetHeightGreedy = texture.height;

        //    ImGui::InputInt("Target Width (Greedy)", &targetWidthGreedy);
        //    ImGui::InputInt("Target Height (Greedy)", &targetHeightGreedy);

        //    // Clamp so we never ask to enlarge the image
        //    targetWidthGreedy = std::clamp(targetWidthGreedy, 1, texture.width);
        //    targetHeightGreedy = std::clamp(targetHeightGreedy, 1, texture.height);

        //    if (ImGui::Button("Resize Image (Greedy)"))
        //    {
        //        // Do the whole resize in one go (no per-frame animation)
        //        while ((texture.width > targetWidthGreedy) ||
        //            (texture.height > targetHeightGreedy))
        //        {
        //            Grid<float> energy = DP::ComputeEnergy(texture);

        //            std::vector<int> vSeam;
        //            std::vector<int> hSeam;
        //            float vEnergy = std::numeric_limits<float>::max();
        //            float hEnergy = std::numeric_limits<float>::max();

        //            // Only consider vertical seam if we still need to shrink width
        //            if (texture.width > targetWidthGreedy)
        //            {
        //                vSeam = Greedy::FindVerticalSeam_Greedy(energy);
        //                vEnergy = DP::CalculateVerticalSeamEnergy(energy, vSeam);
        //            }

        //            // Only consider horizontal seam if we still need to shrink height
        //            if (texture.height > targetHeightGreedy)
        //            {
        //                hSeam = Greedy::FindHorizontalSeam_Greedy(energy);
        //                hEnergy = DP::CalculateHorizontalSeamEnergy(energy, hSeam);
        //            }

        //            // If neither is valid, we’re done
        //            if (vEnergy == std::numeric_limits<float>::max() &&
        //                hEnergy == std::numeric_limits<float>::max())
        //            {
        //                break;
        //            }

        //            // Remove whichever seam has lower total energy (still a valid “resize” policy)
        //            if (vEnergy < hEnergy)
        //                Greedy::RemoveVerticalSeam(texture, vSeam);
        //            else
        //                Greedy::RemoveHorizontalSeam(texture, hSeam);
        //        }

        //        // Single upload at the end = much faster than per-seam updates
        //        UpdateTexture(texture);

        //        std::cout << "Greedy resizing completed. Final size: "
        //            << texture.width << "x" << texture.height << std::endl;
        //    }
        //}
        //ImGui::End();


        ImGui::EndDisabled();

        gui.EndFrame();
        app.Render();
    }
    
    // clean up
    gui.Shutdown();
    app.Shutdown();
    return 0;
}