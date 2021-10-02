#include "VulkanBuffer.h"
#include "SingleTimeCommands.h"

namespace Vulkan
{
    VulkanBuffer::VulkanBuffer(const VulkanDevice& device, size_t size, VkBufferUsageFlags usageFlags) : m_Device(device)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usageFlags;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        CheckResult(vkCreateBuffer(m_Device.GetHandle(), &bufferInfo, nullptr, &m_Buffer), "Buffer Creation");
    }

    VulkanBuffer::~VulkanBuffer()
    {
        if (m_Buffer != nullptr)
        {
            vkDestroyBuffer(m_Device.GetHandle(), m_Buffer, nullptr);
            m_Buffer = nullptr;
        }
    }

    VulkanDeviceMemory VulkanBuffer::AllocateMemory(VkMemoryPropertyFlags propertyFlags)
    {
        return AllocateMemory(0, propertyFlags);
    }

    VulkanDeviceMemory VulkanBuffer::AllocateMemory(VkMemoryAllocateFlags allocationFlags, VkMemoryPropertyFlags propertyFlags)
    {
        const VkMemoryRequirements memoryRequirements = GetMemoryRequirements();
        VulkanDeviceMemory memory(m_Device, memoryRequirements.size, memoryRequirements.memoryTypeBits, allocationFlags, propertyFlags);

        CheckResult(vkBindBufferMemory(m_Device.GetHandle(), m_Buffer, memory.GetHandle(), 0), "Buffer Memory Binding");

        return memory;
    }

    VkMemoryRequirements VulkanBuffer::GetMemoryRequirements() const
    {
        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(m_Device.GetHandle(), m_Buffer, &memoryRequirements);

        return memoryRequirements;
    }

    VkDeviceAddress VulkanBuffer::GetDeviceAddress() const
    {
        // Queries the address of the buffer on the GPU.
        VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {};
        bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        bufferDeviceAddressInfo.pNext = nullptr;
        bufferDeviceAddressInfo.buffer = GetHandle();

        return vkGetBufferDeviceAddress(m_Device.GetHandle(), &bufferDeviceAddressInfo);
    }

    void VulkanBuffer::CopyFrom(VulkanCommandPool& commandPool, const VulkanBuffer& source, VkDeviceSize size)
    {
        SingleTimeCommands::Submit(commandPool, [&](VkCommandBuffer commandBuffer)
        {
            VkBufferCopy copyRegion = {};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = size;

            vkCmdCopyBuffer(commandBuffer, source.GetHandle(), m_Buffer, 1, &copyRegion);
        });
    }
}