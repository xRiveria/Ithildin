#pragma once
#include "../Core/Core.h"

namespace Raytracing
{
    class VulkanInstance;

    class VulkanDebugMessenger final
    {
    public:
        VulkanDebugMessenger(const VulkanInstance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold);
        ~VulkanDebugMessenger();

        VkDebugUtilsMessageSeverityFlagBitsEXT GetThreshold() const { return m_Threshold; }

    private:
        const VulkanInstance& m_Instance;
        const VkDebugUtilsMessageSeverityFlagBitsEXT m_Threshold;

        VULKAN_HANDLE(VkDebugUtilsMessengerEXT, m_Messenger)
    };
}