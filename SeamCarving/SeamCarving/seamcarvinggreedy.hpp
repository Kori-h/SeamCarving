#pragma once

namespace Greedy
{
	std::vector<int> FindVerticalSeam_Greedy(Grid<float> const& energy);
	void RemoveVerticalSeam(Texture& texture, std::vector<int> const& seam);

	std::vector<int> FindHorizontalSeam_Greedy(Grid<float> const& energy);
	void RemoveHorizontalSeam(Texture& texture, std::vector<int> const& seam);
}
