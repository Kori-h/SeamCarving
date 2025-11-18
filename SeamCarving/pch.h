#pragma once

// graphics
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// gui
#include <imgui.h>

// gl gui stuff
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

// image loading
#include <stb_image.h>
#pragma warning(push)
#pragma warning(disable : 4996) // disable specific warning when including this lib
#include <stb_image_write.h>
#pragma warning(pop)

// c++
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <iomanip>

// containers
union Pixel
{
    struct 
    {
        unsigned char r, g, b, a;
    };

    unsigned char data[4];
};

struct Texture
{
    GLuint id;
    int width;
    int height;
    std::vector<Pixel> pixels;

    void SetPixel(int x, int y, Pixel pixel)
    {
        if (x < 0 || x >= width || y < 0 || y >= height)
        {
            return;
        }

        pixels[y * width + x] = pixel;
    }

    Pixel GetPixel(int x, int y) const
    {
        x = std::clamp(x, 0, width - 1);
        y = std::clamp(y, 0, height - 1);
        return pixels[y * width + x];
    }
};

template <typename T>
struct Grid 
{
    int width, height;
    std::vector<T> data;

    Grid(int w, int h, T defaultValue = T{}) : width(w), height(h), data(w * h, defaultValue)
    {

    }

    T& operator()(int x, int y) 
    {
        x = std::clamp(x, 0, width - 1);
        y = std::clamp(y, 0, height - 1);
        return data[y * width + x];
    }

    T const& operator()(int x, int y) const 
    {
        x = std::clamp(x, 0, width - 1);
        y = std::clamp(y, 0, height - 1);
        return data[y * width + x];
    }
};