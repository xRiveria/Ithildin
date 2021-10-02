#pragma once
#include "VulkanDescriptorBinding.h"
#include <vector>

namespace Vulkan
{
    class VulkanDevice;

    class VulkanDescriptorSetLayout final
    {
    public:
        VulkanDescriptorSetLayout(const VulkanDevice& device, const std::vector<VulkanDescriptorBinding>& descriptorBindings);
        ~VulkanDescriptorSetLayout();

    private:
        const VulkanDevice& m_Device;
        VULKAN_HANDLE(VkDescriptorSetLayout, m_Layout)
    };
}