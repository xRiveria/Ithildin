#include "VulkanDescriptorSets.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDevice.h"
#include <stdexcept>

namespace Vulkan
{
    VulkanDescriptorSets::VulkanDescriptorSets(const VulkanDescriptorPool& descriptorPool, const VulkanDescriptorSetLayout& descriptorSetLayout, std::map<uint32_t, VkDescriptorType> bindingTypes, size_t size)
                                             : m_DescriptorPool(descriptorPool), m_BindingTypes(std::move(bindingTypes))
    {
        std::vector<VkDescriptorSetLayout> layouts(size, descriptorSetLayout.GetHandle()); // Reuse the same layout for our sets allocation.

        VkDescriptorSetAllocateInfo allocationInfo = {};
        allocationInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocationInfo.descriptorPool = descriptorPool.GetHandle();
        allocationInfo.descriptorSetCount = static_cast<uint32_t>(size);
        allocationInfo.pSetLayouts = layouts.data();

        m_DescriptorSets.resize(size);

        CheckResult(vkAllocateDescriptorSets(descriptorPool.GetDevice().GetHandle(), &allocationInfo, m_DescriptorSets.data()), "Descriptor Sets Allocation");
    }

    VulkanDescriptorSets::~VulkanDescriptorSets()
    {
    }

    VkWriteDescriptorSet VulkanDescriptorSets::Bind(uint32_t index, uint32_t binding, const VkDescriptorBufferInfo& bufferInfo, uint32_t count) const
    {
        VkWriteDescriptorSet descriptorWriteInfo = {};
        descriptorWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWriteInfo.dstSet = m_DescriptorSets[index]; // Set to update.
        descriptorWriteInfo.dstBinding = binding;             // Descriptor binding within the set.
        descriptorWriteInfo.dstArrayElement = 0;              // Starting element.
        descriptorWriteInfo.descriptorType = GetBindingType(binding);
        descriptorWriteInfo.descriptorCount = count;          // The number of descriptors to update.
        descriptorWriteInfo.pBufferInfo = &bufferInfo;

        return descriptorWriteInfo;
    }

    VkWriteDescriptorSet VulkanDescriptorSets::Bind(uint32_t index, uint32_t binding, const VkDescriptorImageInfo& imageInfo, uint32_t count) const
    {
        VkWriteDescriptorSet descriptorWriteInfo = {};
        descriptorWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWriteInfo.dstSet = m_DescriptorSets[index];
        descriptorWriteInfo.dstBinding = binding;
        descriptorWriteInfo.dstArrayElement = 0;
        descriptorWriteInfo.descriptorType = GetBindingType(binding);
        descriptorWriteInfo.descriptorCount = count;
        descriptorWriteInfo.pImageInfo = &imageInfo;

        return descriptorWriteInfo;
    }

    VkWriteDescriptorSet VulkanDescriptorSets::Bind(uint32_t index, uint32_t binding, const VkWriteDescriptorSetAccelerationStructureKHR& structureInfo, uint32_t count) const
    {
        VkWriteDescriptorSet descriptorWriteInfo = {};
        descriptorWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWriteInfo.dstSet = m_DescriptorSets[index];
        descriptorWriteInfo.dstBinding = binding;
        descriptorWriteInfo.dstArrayElement = 0;
        descriptorWriteInfo.descriptorType = GetBindingType(binding);
        descriptorWriteInfo.descriptorCount = count;
        descriptorWriteInfo.pNext = &structureInfo;

        return descriptorWriteInfo;
    }

    void VulkanDescriptorSets::UpdateDescriptors(uint32_t index, const std::vector<VkWriteDescriptorSet>& descriptorWrites)
    {
        vkUpdateDescriptorSets(m_DescriptorPool.GetDevice().GetHandle(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    VkDescriptorType VulkanDescriptorSets::GetBindingType(uint32_t binding) const
    {
        const auto iterator = m_BindingTypes.find(binding);
        if (iterator == m_BindingTypes.end())
        {
            std::invalid_argument("Binding not found.\n");
        }

        return iterator->second;
    }
}