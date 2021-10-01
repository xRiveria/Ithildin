#include "VulkanCommandPool.h"
#include "VulkanDevice.h"

namespace Raytracing
{
    VulkanCommandPool::VulkanCommandPool(const VulkanDevice& device, uint32_t queueFamilyIndex, bool allowReset) : m_Device(device)
    {
        VkCommandPoolCreateInfo poolCreationInfo = {};
        poolCreationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreationInfo.queueFamilyIndex = queueFamilyIndex; // One type of pool per queue family.
        poolCreationInfo.flags = allowReset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0; // Command buffers created may be reset if true.

        CheckResult(vkCreateCommandPool(m_Device.GetHandle(), &poolCreationInfo, nullptr, &m_CommandPool), "Command Pool Creation");
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        if (m_CommandPool != nullptr)
        {
            vkDestroyCommandPool(m_Device.GetHandle(), m_CommandPool, nullptr);
            m_CommandPool = nullptr;
        }
    }
}