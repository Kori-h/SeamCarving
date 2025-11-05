#pragma once

namespace DP
{
	Grid<float> ComputeEnergy(Texture const& texture);

	std::vector<int> FindVerticalSeam(Grid<float> const& energy);
	float CalculateVerticalSeamEnergy(Grid<float> const& energy, std::vector<int> const& seam);
	void RemoveVerticalSeam(Texture& texture, std::vector<int> const& seam);

	std::vector<int> FindHorizontalSeam(Grid<float> const& energy);
	float CalculateHorizontalSeamEnergy(Grid<float> const& energy, std::vector<int> const& seam);
	void RemoveHorizontalSeam(Texture& texture, std::vector<int> const& seam);
}