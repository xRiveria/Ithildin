#pragma once
#include "Core/Core.h"

namespace Vulkan
{
    class VulkanDescriptorSetLayout;
    class VulkanDevice;

    class VulkanPipelineLayout final
    {
    public:
        VulkanPipelineLayout(const VulkanDevice& device, const VulkanDescriptorSetLayout& descriptorSetLayout);
        ~VulkanPipelineLayout();

    private:
        const VulkanDevice& m_Device;
        VULKAN_HANDLE(VkPipelineLayout, m_PipelineLayout)
    };
}