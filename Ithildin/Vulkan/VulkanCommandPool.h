#pragma once
#include "../Core/Core.h"

namespace Raytracing
{
    class VulkanDevice;

    class VulkanCommandPool final
    {
    public:
        VulkanCommandPool(const VulkanDevice& device, uint32_t queueFamilyIndex, bool allowReset);
        ~VulkanCommandPool();

        const VulkanDevice& GetDevice() const { return m_Device; }

    private:
        const VulkanDevice& m_Device;
        VULKAN_HANDLE(VkCommandPool, m_CommandPool)
    };
}