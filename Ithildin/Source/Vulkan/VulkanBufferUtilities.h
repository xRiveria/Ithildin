#pragma once
#include "VulkanBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanDeviceMemory.h"
#include <memory>
#include <string>
#include <vector>

namespace Vulkan
{
    class VulkanBufferUtilities final
    {
    public:
        template<typename T>
        static void CopyFromStagingBuffer(VulkanCommandPool& commandPool, VulkanBuffer& destinationBuffer, const std::vector<T>& content);

        template<typename T>
        static void CreateDeviceBuffer(VulkanCommandPool& commandPool, const char* name, VkBufferUsageFlags usageFlags, 
                                       const std::vector<T>& content, std::unique_ptr<VulkanBuffer>& buffer, std::unique_ptr<VulkanDeviceMemory>& memory);
    };

    template <typename T>
    void VulkanBufferUtilities::CopyFromStagingBuffer(VulkanCommandPool& commandPool, VulkanBuffer& destinationBuffer, const std::vector<T>& content)
    {
        const VulkanDevice& device = commandPool.GetDevice();
        const size_t contentSize = sizeof(content[0]) * content.size();

        // Create a temporary host visible staging buffer.
        std::unique_ptr<VulkanBuffer> stagingBuffer = std::make_unique<VulkanBuffer>(device, contentSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        VulkanDeviceMemory stagingBufferMemory = stagingBuffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        // Copy the host data into the staging buffer.
        const auto pointerToGPU = stagingBufferMemory.Map(0, contentSize);
        std::memcpy(pointerToGPU, content.data(), contentSize);
        stagingBufferMemory.Unmap();

        // Copy the staging buffer to the GPU (device) buffer.
        destinationBuffer.CopyFrom(commandPool, *stagingBuffer, contentSize);

        // Delete the buffer before the memory (scope exit).
        stagingBuffer.reset();
    }

    template<typename T>
    void VulkanBufferUtilities::CreateDeviceBuffer(VulkanCommandPool& commandPool, const char* name, VkBufferUsageFlags usageFlags,
                            const std::vector<T>& content, std::unique_ptr<VulkanBuffer>& buffer, std::unique_ptr<VulkanDeviceMemory>& memory)
    {
        const VulkanDevice& device = commandPool.GetDevice();
        const VulkanDebugUtilities& debugUtilities = device.GetDebugUtilities();
        const size_t contentSize = sizeof(content[0]) * content.size();
        const VkMemoryAllocateFlags allocateFlags = usageFlags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT ? VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT : 0;

        buffer.reset(new VulkanBuffer(device, contentSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageFlags));
        memory.reset(new VulkanDeviceMemory(buffer->AllocateMemory(allocateFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

        debugUtilities.SetObjectName(buffer->GetHandle(), (name + std::string(" Buffer")).c_str());
        debugUtilities.SetObjectName(memory->GetHandle(), (name + std::string(" Memory")).c_str());

        CopyFromStagingBuffer(commandPool, *buffer, content);
    }
}