#include "VulkanRaytracingProperties.h"
#include "../VulkanDevice.h"
#include <iostream>

namespace Vulkan::Raytracing
{
    VulkanRaytracingProperties::VulkanRaytracingProperties(const VulkanDevice& device) : m_Device(device)
    {
        m_AccelerationStructureProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
        m_RaytracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

        m_RaytracingPipelineProperties.pNext = &m_AccelerationStructureProperties;

        VkPhysicalDeviceProperties2 deviceProperties2 = {};
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties2.pNext = &m_RaytracingPipelineProperties;

        vkGetPhysicalDeviceProperties2(device.GetPhysicalDevice(), &deviceProperties2);
        std::cout << "Successfully retrieved AS and Raytracing Pipeline properties.\n";
    }
}