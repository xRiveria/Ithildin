#pragma once
#include "../Core/Core.h"

namespace Raytracing
{
    class VulkanDevice;

    class VulkanFence final
    {
    public:
        explicit VulkanFence(const VulkanDevice& device, bool isSignaled);
        VulkanFence(VulkanFence&& otherFence) noexcept;
        ~VulkanFence();

        const VulkanDevice& GetDevice() const { return m_Device; }
        const VkFence& GetHandle() const { return m_Fence; }

        void Reset();
        void Wait(uint64_t timeout) const;

    private:
        const VulkanDevice& m_Device;
        VkFence m_Fence{};
    };

}