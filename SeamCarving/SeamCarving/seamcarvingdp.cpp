#include "../pch.h"
#include "seamcarvingdp.hpp"

namespace DP
{
    Grid<float> ComputeEnergy(Texture const& texture)
    {
        Grid<float> energy(texture.width, texture.height, 0.0f);

        for (int y = 0; y < texture.height; ++y)
        {
            for (int x = 0; x < texture.width; ++x)
            {
                Pixel left = texture.GetPixel(x - 1, y);
                Pixel right = texture.GetPixel(x + 1, y);
                Pixel up = texture.GetPixel(x, y - 1);
                Pixel down = texture.GetPixel(x, y + 1);

                float dx = std::pow(float(right.r) - float(left.r), 2) +
                    std::pow(float(right.g) - float(left.g), 2) +
                    std::pow(float(right.b) - float(left.b), 2);

                float dy = std::pow(float(down.r) - float(up.r), 2) +
                    std::pow(float(down.g) - float(up.g), 2) +
                    std::pow(float(down.b) - float(up.b), 2);

                energy(x, y) = std::sqrt(dx + dy);
            }
        }

        return energy;
    }

    std::vector<int> FindVerticalSeam(Grid<float> const& energy)
    {
        int width = energy.width;
        int height = energy.height;

        // Calculate cumulative energy using DP
        Grid<float> cumulative(width, height);

        for (int x = 0; x < width; ++x)
        {
            cumulative(x, 0) = energy(x, 0);
        }

        for (int y = 1; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                float left = (x > 0) ? cumulative(x - 1, y - 1) : std::numeric_limits<float>::max();
                float up = cumulative(x, y - 1);
                float right = (x < width - 1) ? cumulative(x + 1, y - 1) : std::numeric_limits<float>::max();
                cumulative(x, y) = energy(x, y) + std::min({ left, up, right });
            }
        }

        // Find minimum seam
        std::vector<int> seam(height);

        float minEnergy = std::numeric_limits<float>::max();
        int minX = 0;

        for (int x = 0; x < width; ++x)
        {
            if (cumulative(x, height - 1) < minEnergy)
            {
                minEnergy = cumulative(x, height - 1);
                minX = x;
            }
        }

        seam[height - 1] = minX;

        for (int y = height - 2; y >= 0; --y)
        {
            int prevX = seam[y + 1];
            int bestX = prevX;
            float bestEnergy = cumulative(prevX, y);

            if (prevX > 0 && cumulative(prevX - 1, y) < bestEnergy)
            {
                bestX = prevX - 1;
                bestEnergy = cumulative(prevX - 1, y);
            }

            if (prevX < width - 1 && cumulative(prevX + 1, y) < bestEnergy)
            {
                bestX = prevX + 1;
                bestEnergy = cumulative(prevX + 1, y);
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
        std::cout << "Removed vertical seam. New size: " << texture.width << "x" << texture.height << std::endl;
    }

    std::vector<int> FindHorizontalSeam(Grid<float> const& energy)
    {
        int width = energy.width;
        int height = energy.height;

        // DP table for cumulative energy
        Grid<float> cumulative(width, height);

        for (int y = 0; y < height; ++y)
        {
            cumulative(0, y) = energy(0, y);
        }

        for (int x = 1; x < width; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                float up = (y > 0) ? cumulative(x - 1, y - 1) : std::numeric_limits<float>::max();
                float left = cumulative(x - 1, y);
                float down = (y < height - 1) ? cumulative(x - 1, y + 1) : std::numeric_limits<float>::max();

                cumulative(x, y) = energy(x, y) + std::min({ up, left, down });
            }
        }

        // Find minimum seam
        std::vector<int> seam(width);
        float minEnergy = std::numeric_limits<float>::max();
        int minY = 0;

        for (int y = 0; y < height; ++y)
        {
            if (cumulative(width - 1, y) < minEnergy)
            {
                minEnergy = cumulative(width - 1, y);
                minY = y;
            }
        }

        seam[width - 1] = minY;

        for (int x = width - 2; x >= 0; --x)
        {
            int prevY = seam[x + 1];
            int bestY = prevY;
            float bestEnergy = cumulative(x, prevY);

            if (prevY > 0 && cumulative(x, prevY - 1) < bestEnergy)
            {
                bestY = prevY - 1;
                bestEnergy = cumulative(x, prevY - 1);
            }

            if (prevY < height - 1 && cumulative(x, prevY + 1) < bestEnergy)
            {
                bestY = prevY + 1;
                bestEnergy = cumulative(x, prevY + 1);
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
        std::cout << "Removed horizontal seam. New size: " << texture.width << "x" << texture.height << std::endl;
    }
}