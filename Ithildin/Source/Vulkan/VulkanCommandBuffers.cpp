#include "VulkanCommandBuffers.h"
#include "VulkanCommandPool.h"
#include "VulkanDevice.h"

namespace Vulkan
{
    VulkanCommandBuffers::VulkanCommandBuffers(VulkanCommandPool& commandPool, uint32_t size) : m_CommandPool(commandPool)
    {
        VkCommandBufferAllocateInfo allocationInfo = {};
        allocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocationInfo.commandPool = commandPool.GetHandle();
        allocationInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocationInfo.commandBufferCount = size;

        m_CommandBuffers.resize(size);

        CheckResult(vkAllocateCommandBuffers(commandPool.GetDevice().GetHandle(), &allocationInfo, m_CommandBuffers.data()), "Command Buffers Allocation");
    }

    VulkanCommandBuffers::~VulkanCommandBuffers()
    {
        if (!m_CommandBuffers.empty())
        {
            vkFreeCommandBuffers(m_CommandPool.GetDevice().GetHandle(), m_CommandPool.GetHandle(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
            m_CommandBuffers.clear();
        }
    }

    VkCommandBuffer VulkanCommandBuffers::BeginRecording(size_t i)
    {
        VkCommandBufferBeginInfo beginInfoDescription = {};
        beginInfoDescription.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfoDescription.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // Allows us to submit the command buffer multiple times without waiting for the previous submit to finish via a fence.
        beginInfoDescription.pInheritanceInfo = nullptr; // Optional

        CheckResult(vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfoDescription), "Begin Command Buffer");

        return m_CommandBuffers[i];
    }

    void VulkanCommandBuffers::EndRecording(size_t i)
    {
        CheckResult(vkEndCommandBuffer(m_CommandBuffers[i]), "End Command Buffer");
    }
}