#include "VulkanFence.h"
#include "VulkanDevice.h"

namespace Vulkan
{
    VulkanFence::VulkanFence(const VulkanDevice& device, bool isSignaled) : m_Device(device)
    {
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = isSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        CheckResult(vkCreateFence(m_Device.GetHandle(), &fenceInfo, nullptr, &m_Fence), "Fence Creation");
    }

    VulkanFence::VulkanFence(VulkanFence&& otherFence) noexcept : m_Device(otherFence.m_Device), m_Fence(otherFence.m_Fence)
    {
        otherFence.m_Fence = nullptr;
    }
    
    VulkanFence::~VulkanFence()
    {
        if (m_Fence != nullptr)
        {
            vkDestroyFence(m_Device.GetHandle(), m_Fence, nullptr);
            m_Fence = nullptr;
        }
    }

    void VulkanFence::Reset()
    {
        CheckResult(vkResetFences(m_Device.GetHandle(), 1, &m_Fence), "Fence Reset");
    }

    void VulkanFence::Wait(uint64_t timeout) const
    {
        CheckResult(vkWaitForFences(m_Device.GetHandle(), 1, &m_Fence, VK_TRUE, timeout), "Wait For Fence");
    }
}