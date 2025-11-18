#pragma once

namespace Greedy
{
	std::vector<int> FindVerticalSeamGreedy(Grid<float> const& energy);
	void RemoveVerticalSeam(Texture& texture, std::vector<int> const& seam);

	std::vector<int> FindHorizontalSeamGreedy(Grid<float> const& energy);
	void RemoveHorizontalSeam(Texture& texture, std::vector<int> const& seam);
}
