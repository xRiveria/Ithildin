#pragma once
#include "../Core/Core.h"

namespace Vulkan
{
    class VulkanDevice;

    class VulkanDeviceMemory final
    {
    public:
        VulkanDeviceMemory(const VulkanDevice& device, size_t size, uint32_t memoryTypeBits, VkMemoryAllocateFlags allocationFlags, VkMemoryPropertyFlags propertyFlags);
        VulkanDeviceMemory(VulkanDeviceMemory&& otherMemory) noexcept; // Terminate if an exception is thrown at runtime.
        ~VulkanDeviceMemory();

        const VulkanDevice& GetDevice() const { return m_Device; }

        void* Map(size_t offset, size_t size);
        void Unmap();

    private:
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) const;

    private:
        const VulkanDevice& m_Device;
        VULKAN_HANDLE(VkDeviceMemory, m_Memory)
    };
}