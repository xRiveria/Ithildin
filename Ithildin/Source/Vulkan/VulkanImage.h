#pragma once
#include "../Core/Core.h"
#include "VulkanDeviceMemory.h"

namespace Vulkan
{
    class VulkanBuffer;
    class VulkanCommandPool;
    class VulkanDevice;

    class VulkanImage final
    {
    public:
        VulkanImage(const VulkanDevice& device, VkExtent2D extent, VkFormat format);
        VulkanImage(const VulkanDevice& device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags);
        VulkanImage(VulkanImage&& otherImage) noexcept;
        ~VulkanImage();

        const VulkanDevice& GetDevice() const { return m_Device; }
        VkExtent2D GetExtent() const { return m_Extent; }
        VkFormat GetFormat() const { return m_Format; }

        VulkanDeviceMemory AllocateMemory(VkMemoryPropertyFlags propertyFlags) const;
        VkMemoryRequirements GetMemoryRequirements() const;

        void TransitionImageLayout(VulkanCommandPool& commandPool, VkImageLayout newLayout);
        void CopyFrom(VulkanCommandPool& commandPool, const VulkanBuffer& buffer);

    private:
        const VulkanDevice& m_Device;
        const VkExtent2D m_Extent;
        const VkFormat m_Format;
        VkImageLayout m_ImageLayout;

        VULKAN_HANDLE(VkImage, m_Image)
    };
}