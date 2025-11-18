#include "../pch.h"
#include "seamcarvinggreedy.hpp"

namespace Greedy
{
    std::vector<int> FindVerticalSeamGreedy(Grid<float> const& energy)
    {
        int width = energy.width;
        int height = energy.height;

        std::vector<int> seam(height);

        // Start from the pixel with the minimum energy in the top row
        int x = 0;
        float minEnergy = std::numeric_limits<float>::max();
        for (int i = 0; i < width; ++i)
        {
            if (energy(i, 0) < minEnergy)
            {
                minEnergy = energy(i, 0);
                x = i;
            }
        }

        seam[0] = x;

        // Greedy: At each row, pick the lowest-energy neighbor from the row below
        for (int y = 1; y < height; ++y)
        {
            int bestX = x;
            float bestVal = energy(x, y);

            if (x > 0 && energy(x - 1, y) < bestVal)
            {
                bestVal = energy(x - 1, y);
                bestX = x - 1;
            }
            if (x < width - 1 && energy(x + 1, y) < bestVal)
            {
                bestVal = energy(x + 1, y);
                bestX = x + 1;
            }

            x = bestX;
            seam[y] = x;
        }

        return seam;
    }

    std::vector<int> FindHorizontalSeamGreedy(Grid<float> const& energy)
    {
        int width = energy.width;
        int height = energy.height;

        std::vector<int> seam(width);

        // Start from the pixel with the minimum energy in the leftmost column
        int y = 0;
        float minEnergy = std::numeric_limits<float>::max();
        for (int i = 0; i < height; ++i)
        {
            if (energy(0, i) < minEnergy)
            {
                minEnergy = energy(0, i);
                y = i;
            }
        }

        seam[0] = y;

        // Greedy: At each column, pick the lowest-energy neighbor from the next column
        for (int x = 1; x < width; ++x)
        {
            int bestY = y;
            float bestVal = energy(x, y);

            if (y > 0 && energy(x, y - 1) < bestVal)
            {
                bestVal = energy(x, y - 1);
                bestY = y - 1;
            }
            if (y < height - 1 && energy(x, y + 1) < bestVal)
            {
                bestVal = energy(x, y + 1);
                bestY = y + 1;
            }

            y = bestY;
            seam[x] = y;
        }

        return seam;
    }
}