#pragma once
#include "Vulkan/VulkanSampler.h"
#include <memory>
#include <string>

namespace Resources
{
    class Texture final
    {
    public:
        static Texture LoadTexture(const std::string& filePath, const Raytracing::SamplerConfiguration& samplerConfiguration);

        Texture() = default;
        Texture(const Texture& texture) = default;
        Texture(Texture&& texture) = default;

        const unsigned char* GetPixels() const { return m_Pixels.get(); }
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }

    private:
        Texture(int width, int height, int channels, unsigned char* pixels);

    private:
        Raytracing::SamplerConfiguration m_SamplerConfiguration;
        int m_Width;
        int m_Height;
        int m_Channels;
        std::unique_ptr<unsigned char, void(*)(void* stbiValueToDelete)> m_Pixels;
    };
 
}