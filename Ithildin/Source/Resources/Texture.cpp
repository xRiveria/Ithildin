#include "Texture.h"
#include <iostream>
#include <chrono>
#include "Importers/ImageImporter.h"

namespace Resources
{
    Texture Texture::LoadTexture(const std::string& filePath, const Vulkan::SamplerConfiguration& samplerConfiguration)
    {
        std::cout << "Loading: " << filePath << "...\n";
        const std::chrono::steady_clock::time_point timer = std::chrono::high_resolution_clock::now();

        // Load the texture in normal host memory.
        int width, height, channels;
        const auto pixels = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha); // Force Load RGBA

        if (!pixels)
        {
            std::runtime_error("Failed to load Texture Image: " + filePath);
        }

        const float elapsedTime = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - timer).count();
        std::cout << "(" << width << " x " << height << " x " << channels << ")\n";
        std::cout << "Elapsed: " << elapsedTime << "seconds.\n";

        return Texture(width, height, channels, pixels);
    }

    Texture::Texture(int width, int height, int channels, unsigned char* const pixels)
        : m_Width(width), m_Height(height), m_Channels(channels), m_Pixels(pixels, stbi_image_free)
    {

    }
}