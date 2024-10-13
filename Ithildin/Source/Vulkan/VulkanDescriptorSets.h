#pragma once
#include "Core/Core.h"
#include <vector>
#include <map>

namespace Vulkan
{
    class VulkanBuffer;
    class VulkanDescriptorPool;
    class VulkanDescriptorSetLayout;
    class VulkanImageView;

    class VulkanDescriptorSets final
    {
    public:
        VulkanDescriptorSets(const VulkanDescriptorPool& descriptorPool, const VulkanDescriptorSetLayout& descriptorSetLayout,
                             std::map<uint32_t, VkDescriptorType> bindingTypes, size_t size);
        ~VulkanDescriptorSets();

        VkDescriptorSet GetDescriptorSetHandle(uint32_t index) const { return m_DescriptorSets[index]; }

        // Write Operations
        VkWriteDescriptorSet Bind(uint32_t index, uint32_t binding, const VkDescriptorBufferInfo& bufferInfo, uint32_t count = 1) const;
        VkWriteDescriptorSet Bind(uint32_t index, uint32_t binding, const VkDescriptorImageInfo& imageInfo, uint32_t count = 1) const;
        VkWriteDescriptorSet Bind(uint32_t index, uint32_t binding, const VkWriteDescriptorSetAccelerationStructureKHR& structureInfo, uint32_t count = 1) const;

        void UpdateDescriptors(uint32_t index, const std::vector<VkWriteDescriptorSet>& descriptorWrites);

    private:
        VkDescriptorType GetBindingType(uint32_t binding) const;

    private:
        const VulkanDescriptorPool& m_DescriptorPool;
        const std::map<uint32_t, VkDescriptorType> m_BindingTypes;

        std::vector<VkDescriptorSet> m_DescriptorSets;
    };
}