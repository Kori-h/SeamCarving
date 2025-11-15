#include "../pch.h"
#include "seamcarvinggreedy.hpp"

namespace Greedy
{
    std::vector<int> FindVerticalSeamGreedy(Grid<float> const& energy)
    {
        int width = energy.width;
        int height = energy.height;

        std::vector<int> seam(height);

        // Start by finding the minimum energy pixel in the first row
        float minEnergy = energy(0, 0);
        int minX = 0;
        for (int x = 1; x < width; ++x)
        {
            if (energy(x, 0) < minEnergy)
            {
                minEnergy = energy(x, 0);
                minX = x;
            }
        }
        seam[0] = minX;

        // Greedy choice: at each row, choose the minimum energy neighbor
        for (int y = 1; y < height; ++y)
        {
            int prevX = seam[y - 1];
            int bestX = prevX;
            float bestEnergy = energy(prevX, y);

            // Check left diagonal
            if (prevX > 0 && energy(prevX - 1, y) < bestEnergy)
            {
                bestX = prevX - 1;
                bestEnergy = energy(prevX - 1, y);
            }

            // Check right diagonal
            if (prevX < width - 1 && energy(prevX + 1, y) < bestEnergy)
            {
                bestX = prevX + 1;
                bestEnergy = energy(prevX + 1, y);
            }

            seam[y] = bestX;
        }

        return seam;
    }

    float CalculateVerticalSeamEnergy(Grid<float> const& energy, std::vector<int> const& seam)
    {
        float totalEnergy = 0.0f;
        for (int y = 0; y < energy.height; ++y)
        {
            totalEnergy += energy(seam[y], y);
        }
        return totalEnergy;
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
        std::cout << "Removed vertical seam (Greedy). New size: " << texture.width << "x" << texture.height << std::endl;
    }

    std::vector<int> FindHorizontalSeamGreedy(Grid<float> const& energy)
    {
        int width = energy.width;
        int height = energy.height;

        std::vector<int> seam(width);

        // Start by finding the minimum energy pixel in the first column
        float minEnergy = energy(0, 0);
        int minY = 0;
        for (int y = 1; y < height; ++y)
        {
            if (energy(0, y) < minEnergy)
            {
                minEnergy = energy(0, y);
                minY = y;
            }
        }
        seam[0] = minY;

        // Greedy choice: at each column, choose the minimum energy neighbor
        for (int x = 1; x < width; ++x)
        {
            int prevY = seam[x - 1];
            int bestY = prevY;
            float bestEnergy = energy(x, prevY);

            // Check up diagonal
            if (prevY > 0 && energy(x, prevY - 1) < bestEnergy)
            {
                bestY = prevY - 1;
                bestEnergy = energy(x, prevY - 1);
            }

            // Check down diagonal
            if (prevY < height - 1 && energy(x, prevY + 1) < bestEnergy)
            {
                bestY = prevY + 1;
                bestEnergy = energy(x, prevY + 1);
            }

            seam[x] = bestY;
        }

        return seam;
    }

    float CalculateHorizontalSeamEnergy(Grid<float> const& energy, std::vector<int> const& seam)
    {
        float totalEnergy = 0.0f;
        for (int x = 0; x < energy.width; ++x)
        {
            totalEnergy += energy(x, seam[x]);
        }
        return totalEnergy;
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
        std::cout << "Removed horizontal seam (Greedy). New size: " << texture.width << "x" << texture.height << std::endl;
    }
}