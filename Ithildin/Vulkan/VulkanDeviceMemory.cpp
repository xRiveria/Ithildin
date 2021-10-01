#include "VulkanDeviceMemory.h"
#include "VulkanDevice.h"
#include <stdexcept>

namespace Raytracing
{
    VulkanDeviceMemory::VulkanDeviceMemory(const VulkanDevice& device, size_t size, uint32_t memoryTypeBits, VkMemoryAllocateFlags allocationFlags, VkMemoryPropertyFlags propertyFlags)
                                         : m_Device(device)
    {
        // Contains flags controlling how many instances of the memory will be allocated: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkMemoryAllocateFlagBits.html
        VkMemoryAllocateFlagsInfo flagsInfoDescription = {};
        flagsInfoDescription.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        flagsInfoDescription.pNext = nullptr;
        flagsInfoDescription.flags = allocationFlags;

        VkMemoryAllocateInfo allocationInfoDescription = {};
        allocationInfoDescription.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocationInfoDescription.pNext = &flagsInfoDescription;
        allocationInfoDescription.allocationSize = size;
        allocationInfoDescription.memoryTypeIndex = FindMemoryType(memoryTypeBits, propertyFlags);

        CheckResult(vkAllocateMemory(m_Device.GetHandle(), &allocationInfoDescription, nullptr, &m_Memory), "Memory Allocation");
    }

    VulkanDeviceMemory::VulkanDeviceMemory(VulkanDeviceMemory&& otherMemory) noexcept 
                                          : m_Device(otherMemory.m_Device), m_Memory(otherMemory.m_Memory)
    {
        otherMemory.m_Memory = nullptr;
    }

    VulkanDeviceMemory::~VulkanDeviceMemory()
    {
        if (m_Memory != nullptr)
        {
            vkFreeMemory(m_Device.GetHandle(), m_Memory, nullptr);
            m_Memory = nullptr;
        }
    }

    void* VulkanDeviceMemory::Map(size_t offset, size_t size)
    {
        // We must guarantee that the any previously submitted commands that writes to the incoming mapped range has completed before the host reads from or writes again to that range.
        void* data;
        CheckResult(vkMapMemory(m_Device.GetHandle(), m_Memory, offset, size, 0, &data), "Map Memory"); // Maps a CPU (host) pointer to GPU (device) memory.

        return data;
    }

    void VulkanDeviceMemory::Unmap()
    {
        vkUnmapMemory(m_Device.GetHandle(), m_Memory);
    }

    // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkMemoryPropertyFlagBits.html
    uint32_t VulkanDeviceMemory::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) const 
    {
        VkPhysicalDeviceMemoryProperties memoryProperties; 
        vkGetPhysicalDeviceMemoryProperties(m_Device.GetPhysicalDevice(), &memoryProperties);

        for (uint32_t i = 0; i != memoryProperties.memoryTypeCount; ++i)
        {
            if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
            {
                return i;
            }
        }

        std::runtime_error("Failed to find suitable memory type.\n");
    }
}