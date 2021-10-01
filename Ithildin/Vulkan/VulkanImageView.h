#pragma once
#include "../Core/Core.h"

namespace Raytracing
{
    class VulkanDevice;

    class VulkanImageView final
    {
    public:
        explicit VulkanImageView(const VulkanDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        ~VulkanImageView();

        const VulkanDevice& GetDevice() const { return m_Device; }

    private:
        const VulkanDevice& m_Device;
        const VkImage m_VulkanImage;
        const VkFormat m_VulkanFormat;

        VULKAN_HANDLE(VkImageView, m_ImageView);
    };
}