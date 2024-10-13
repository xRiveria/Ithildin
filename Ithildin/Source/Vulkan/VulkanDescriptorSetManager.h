#pragma once
#include "VulkanDescriptorBinding.h"
#include <memory>
#include <vector>

namespace Vulkan
{
    class VulkanDevice;
    class VulkanDescriptorPool;
    class VulkanDescriptorSetLayout;
    class VulkanDescriptorSets;

    class VulkanDescriptorSetManager final
    {
    public:
        explicit VulkanDescriptorSetManager(const VulkanDevice& device, const std::vector<VulkanDescriptorBinding>& descriptorBindings, size_t maxSets);
        ~VulkanDescriptorSetManager();

        const VulkanDescriptorSetLayout& GetDescriptorSetLayout() const { return *m_DescriptorSetLayout; }
        VulkanDescriptorSets& GetDescriptorSets() const { return *m_DescriptorSets; }

    private:
        std::unique_ptr<VulkanDescriptorPool> m_DescriptorPool;
        std::unique_ptr<VulkanDescriptorSetLayout> m_DescriptorSetLayout;
        std::unique_ptr<VulkanDescriptorSets> m_DescriptorSets;
    };
}