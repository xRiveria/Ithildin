#pragma once
#include "Core/Core.h"
#include "VulkanDescriptorBinding.h"
#include <vector>

namespace Vulkan
{
    class VulkanDevice;

    class VulkanDescriptorPool final
    {
    public:
        VulkanDescriptorPool(const VulkanDevice& device, const std::vector<VulkanDescriptorBinding>& descriptorBindings, size_t maxSets);
        ~VulkanDescriptorPool();

        const VulkanDevice& GetDevice() const { return m_Device; }

    private:
        const VulkanDevice& m_Device;
        VULKAN_HANDLE(VkDescriptorPool, m_DescriptorPool)
    };
}