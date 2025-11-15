#include "pch.h"

// core libs
#include "Core/stbloader.hpp"
#include "Core/glapp.hpp"
#include "Core/gui.hpp"

// seam carving using dynamic programming
#include "SeamCarving/seamcarvingdp.hpp"

// seam carving using greedy algorithm
#include "SeamCarving/seamcarvinggreedy.hpp"

// analysis and comparison tools
#include "SeamCarving/analysis.hpp"

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

        ImGui::Begin("Basic Controls (DP)");
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

        ImGui::Begin("Basic Controls (Greedy)");
        {
            if (ImGui::Button("Remove Horizontal (Greedy)"))
            {
                Grid<float> energy = DP::ComputeEnergy(texture);
                std::vector<int> seam = Greedy::FindHorizontalSeamGreedy(energy);
                Greedy::RemoveHorizontalSeam(texture, seam);
                UpdateTexture(texture);
            }

            if (ImGui::Button("Remove Vertical (Greedy)"))
            {
                Grid<float> energy = DP::ComputeEnergy(texture);
                std::vector<int> seam = Greedy::FindVerticalSeamGreedy(energy);
                Greedy::RemoveVerticalSeam(texture, seam);
                UpdateTexture(texture);
            }

            if (ImGui::Button("Remove Lowest Energy Seam (Greedy)"))
            {
                Grid<float> energy = DP::ComputeEnergy(texture);
                auto vSeam = Greedy::FindVerticalSeamGreedy(energy);
                auto hSeam = Greedy::FindHorizontalSeamGreedy(energy);
                float vEnergy = Greedy::CalculateVerticalSeamEnergy(energy, vSeam);
                float hEnergy = Greedy::CalculateHorizontalSeamEnergy(energy, hSeam);
                if (vEnergy < hEnergy) Greedy::RemoveVerticalSeam(texture, vSeam);
                else Greedy::RemoveHorizontalSeam(texture, hSeam);
                UpdateTexture(texture);
            }
        }
        ImGui::End();

        ImGui::Begin("Analysis & Comparison");
        {
            ImGui::Text("Compare DP vs Greedy Performance");
            ImGui::Separator();

            if (ImGui::Button("Analyze Vertical Seam"))
            {
                Grid<float> energy = DP::ComputeEnergy(texture);

                std::vector<int> dpSeam, greedySeam;

                auto dpMetrics = Analysis::MeasureDPVerticalSeam(energy, dpSeam);
                auto greedyMetrics = Analysis::MeasureGreedyVerticalSeam(energy, greedySeam);

                Analysis::PrintComparisonTable(dpMetrics, greedyMetrics, "Vertical Seam");
                Analysis::CompareSeams(dpSeam, greedySeam, "DP", "Greedy");
            }

            if (ImGui::Button("Analyze Horizontal Seam"))
            {
                Grid<float> energy = DP::ComputeEnergy(texture);

                std::vector<int> dpSeam, greedySeam;

                auto dpMetrics = Analysis::MeasureDPHorizontalSeam(energy, dpSeam);
                auto greedyMetrics = Analysis::MeasureGreedyHorizontalSeam(energy, greedySeam);

                Analysis::PrintComparisonTable(dpMetrics, greedyMetrics, "Horizontal Seam");
                Analysis::CompareSeams(dpSeam, greedySeam, "DP", "Greedy");
            }

            ImGui::Separator();
            ImGui::Text("Theoretical Analysis (Question 2a)");

            static int analysisRows = 10;
            static int analysisCols = 10;

            ImGui::InputInt("Rows (m)", &analysisRows);
            ImGui::InputInt("Cols (n)", &analysisCols);

            analysisRows = std::clamp(analysisRows, 1, 100);
            analysisCols = std::clamp(analysisCols, 1, 100);

            if (ImGui::Button("Calculate Possible Seams"))
            {
                std::cout << "\n=== Theoretical Analysis: Number of Possible Seams ===" << std::endl;
                std::cout << "Image dimensions: " << analysisRows << " rows x " << analysisCols << " cols" << std::endl;

                if (analysisRows <= 20)
                {
                    unsigned long long count = Analysis::CountPossibleSeams(analysisRows, analysisCols);
                    std::cout << "Exact count: " << count << " possible seams" << std::endl;
                }

                double logCount = Analysis::EstimatePossibleSeamsLog(analysisRows, analysisCols);
                std::cout << "Number of possible seams ? 10^" << logCount << std::endl;

                // Show exponential growth
                std::cout << "\nExponential Growth Demonstration:" << std::endl;
                for (int m = 2; m <= std::min(25, analysisRows); m += 2)
                {
                    if (m <= 20)
                    {
                        unsigned long long c = Analysis::CountPossibleSeams(m, analysisCols);
                        std::cout << "  m=" << m << ": " << c << " seams" << std::endl;
                    }
                    else
                    {
                        double logC = Analysis::EstimatePossibleSeamsLog(m, analysisCols);
                        std::cout << "  m=" << m << ": ~10^" << logC << " seams" << std::endl;
                    }
                }

                // Lower bound: at least 2^(m-1)
                std::cout << "\nLower bound: At least 2^(m-1) = 2^" << (analysisRows - 1)
                    << " ? 10^" << ((analysisRows - 1) * std::log10(2)) << std::endl;
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