#pragma once
#include "../Core/Core.h"

namespace Raytracing
{
    class VulkanDevice;

    class VulkanSemaphore final
    {
    public:
        explicit VulkanSemaphore(const VulkanDevice& device);
        VulkanSemaphore(VulkanSemaphore&& otherSemaphore) noexcept;
        ~VulkanSemaphore();

        const VulkanDevice& GetDevice() const { return m_Device; }

    private:
        const VulkanDevice& m_Device;
        VULKAN_HANDLE(VkSemaphore, m_Semaphore)
    };
}