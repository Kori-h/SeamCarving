#pragma once

namespace Greedy
{
    // Greedy algorithm: at each row, pick the minimum energy pixel
    // among the three valid neighbors (left diagonal, straight, right diagonal)
    std::vector<int> FindVerticalSeamGreedy(Grid<float> const& energy);
    float CalculateVerticalSeamEnergy(Grid<float> const& energy, std::vector<int> const& seam);
    void RemoveVerticalSeam(Texture& texture, std::vector<int> const& seam);

    std::vector<int> FindHorizontalSeamGreedy(Grid<float> const& energy);
    float CalculateHorizontalSeamEnergy(Grid<float> const& energy, std::vector<int> const& seam);
    void RemoveHorizontalSeam(Texture& texture, std::vector<int> const& seam);
}