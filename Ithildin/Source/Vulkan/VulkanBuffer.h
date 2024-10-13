#pragma once
#include "../Core/Core.h"
#include "VulkanDeviceMemory.h"

namespace Vulkan
{
    class VulkanCommandPool;
    class VulkanDevice;

    class VulkanBuffer final
    {
    public:
        VulkanBuffer(const VulkanDevice& device, size_t size, VkBufferUsageFlags usageFlags);
        ~VulkanBuffer();

        const VulkanDevice& GetDevice() const { return m_Device; }

        VulkanDeviceMemory AllocateMemory(VkMemoryPropertyFlags propertyFlags);
        VulkanDeviceMemory AllocateMemory(VkMemoryAllocateFlags allocationFlags, VkMemoryPropertyFlags propertyFlags);
        VkMemoryRequirements GetMemoryRequirements() const;
        VkDeviceAddress GetDeviceAddress() const;

        void CopyFrom(VulkanCommandPool& commandPool, const VulkanBuffer& source, VkDeviceSize size);

    private:
        const VulkanDevice& m_Device;
        VULKAN_HANDLE(VkBuffer, m_Buffer)
    };
}