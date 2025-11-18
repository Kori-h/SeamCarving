#pragma once

Texture LoadTexture(std::string const& file);
void UpdateTexture(Texture const& texture);
bool SaveTextureAsPNG(Texture const& texture, std::string const& filename);