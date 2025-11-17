#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../pch.h"
#include "stbloader.hpp"


Texture LoadTexture(std::string const& file)
{
    Texture texture;
    int channels;
    unsigned char* data = stbi_load(file.c_str(), &texture.width, &texture.height, &channels, STBI_rgb_alpha);
    if (!data) 
    { 
        std::cerr << "Failed to load image!" << std::endl;
        return texture;
    }

    texture.pixels.resize(texture.width * texture.height);
    for (int i = 0; i < texture.width * texture.height; ++i)
    {
        std::memcpy(texture.pixels[i].data, data + 4 * i, 4);
    }

    stbi_image_free(data);

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.pixels.data());

    std::cout << "Imaged loaded." << std::endl;
    return texture;
}

void UpdateTexture(Texture const& texture)
{
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.pixels.data());
    std::cout << "Imaged updated." << std::endl;
}

bool SaveTextureAsPNG(Texture const& texture, std::string const& filename)
{
    unsigned char const* data = reinterpret_cast<unsigned char const*>(texture.pixels.data());

    int result = stbi_write_png(
        filename.c_str(),
        texture.width,
        texture.height,
        4,                      // 4 channels: RGBA
        data,
        texture.width * 4       // stride in bytes
    );

    if (result != 0)  std::cout << "Image exported. " << filename << std::endl;
    else std::cout << "Failed to export image" << std::endl;

    return result != 0;
}
