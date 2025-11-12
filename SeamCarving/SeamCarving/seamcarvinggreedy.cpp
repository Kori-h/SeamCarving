#include "../pch.h"
#include "seamcarvinggreedy.hpp"

namespace Greedy
{
    std::vector<int> FindVerticalSeam_Greedy(Grid<float> const& energy)
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
    void RemoveVerticalSeam(Texture& texture, std::vector<int> const& seam)
    {
        if (texture.width <= 1)
        {
            std::cerr << "Cannot remove vertical seam, image is too small!" << std::endl;
            return;
        }

        std::vector<Pixel> newPixels((texture.width - 1) * texture.height);

        for (int y = 0; y < texture.height; ++y)
        {
            int k = 0;
            for (int x = 0; x < texture.width; ++x)
            {
                if (x != seam[y])
                {
                    newPixels[y * (texture.width - 1) + k] = texture.pixels[y * texture.width + x];
                    ++k;
                }
            }
        }

        --texture.width;
        texture.pixels = std::move(newPixels);
        std::cout << "Removed vertical seam. New size: " << texture.width << "x" << texture.height << std::endl;
    }

    std::vector<int> FindHorizontalSeam_Greedy(Grid<float> const& energy)
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

    void RemoveHorizontalSeam(Texture& texture, std::vector<int> const& seam)
    {
        if (texture.height <= 1)
        {
            std::cerr << "Cannot remove horizontal seam, image is too small!" << std::endl;
            return;
        }

        std::vector<Pixel> newPixels(texture.width * (texture.height - 1));

        for (int x = 0; x < texture.width; ++x)
        {
            int k = 0;
            for (int y = 0; y < texture.height; ++y)
            {
                if (y != seam[x])
                {
                    newPixels[k * texture.width + x] = texture.pixels[y * texture.width + x];
                    k++;
                }
            }
        }

        --texture.height;
        texture.pixels = std::move(newPixels);
        std::cout << "Removed horizontal seam. New size: " << texture.width << "x" << texture.height << std::endl;
    }
}