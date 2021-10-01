#pragma once
#include "../Core/Core.h"
#include <memory>

namespace Raytracing
{
    class VulkanCommandPool;
    class VulkanDevice;
    class VulkanDeviceMemory;
    class VulkanImage;
    class VulkanImageView;

    class VulkanDepthBuffer final
    {
    public:
        VulkanDepthBuffer(VulkanCommandPool& commandPool, VkExtent2D extent);
        ~VulkanDepthBuffer();

        VkFormat GetFormat() const { return m_Format; }
        const VulkanImageView& GetImageView() const { return *m_ImageView; }

        static bool HasStencilComponent(const VkFormat format)
        {
            return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
        }

    private:
        const VkFormat m_Format;
        std::unique_ptr<VulkanImage> m_Image;
        std::unique_ptr<VulkanDeviceMemory> m_ImageMemory;
        std::unique_ptr<VulkanImageView> m_ImageView;
    };
}