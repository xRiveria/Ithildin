#pragma once
#include <memory>

namespace Raytracing
{
    class VulkanCommandPool;
    class VulkanDeviceMemory;
    class VulkanImage;
    class VulkanImageView;
    class VulkanSampler;
}

namespace Resources
{
    class Texture;

    class TextureImage final
    {
    public:
        TextureImage(Raytracing::VulkanCommandPool& commandPool, const Texture& texture);
        ~TextureImage();

        const Raytracing::VulkanImageView& GetImageView() const { return *m_ImageView; }
        const Raytracing::VulkanSampler& GetSampler() const { return *m_ImageSampler; }

    private:
        std::unique_ptr<Raytracing::VulkanImage> m_Image;
        std::unique_ptr<Raytracing::VulkanDeviceMemory> m_ImageMemory;
        std::unique_ptr<Raytracing::VulkanImageView> m_ImageView;
        std::unique_ptr<Raytracing::VulkanSampler> m_ImageSampler;
    };
}