#pragma once
#include <memory>

namespace Vulkan
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
        TextureImage(Vulkan::VulkanCommandPool& commandPool, const Texture& texture);
        ~TextureImage();

        const Vulkan::VulkanImageView& GetImageView() const { return *m_ImageView; }
        const Vulkan::VulkanSampler& GetSampler() const { return *m_ImageSampler; }

    private:
        std::unique_ptr<Vulkan::VulkanImage> m_Image;
        std::unique_ptr<Vulkan::VulkanDeviceMemory> m_ImageMemory;
        std::unique_ptr<Vulkan::VulkanImageView> m_ImageView;
        std::unique_ptr<Vulkan::VulkanSampler> m_ImageSampler;
    };
}