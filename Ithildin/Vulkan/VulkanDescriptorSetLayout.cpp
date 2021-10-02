#include "VulkanDescriptorSetLayout.h"
#include "VulkanDevice.h"

namespace Vulkan
{
    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VulkanDevice& device, const std::vector<VulkanDescriptorBinding>& descriptorBindings)
                                                       : m_Device(device)
    {
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

        for (const VulkanDescriptorBinding& binding : descriptorBindings)
        {
            VkDescriptorSetLayoutBinding internalBinding = {};
            internalBinding.binding = binding.m_BindingIndex;
            internalBinding.descriptorCount = binding.m_DescriptorCount;
            internalBinding.descriptorType = binding.m_Type;
            internalBinding.stageFlags = binding.m_StageFlags;

            layoutBindings.push_back(internalBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
        layoutCreateInfo.pBindings = layoutBindings.data();

        CheckResult(vkCreateDescriptorSetLayout(device.GetHandle(), &layoutCreateInfo, nullptr, &m_Layout), "Descriptor Layout Creation");
    }

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
    {
        if (m_Layout != nullptr)
        {
            vkDestroyDescriptorSetLayout(m_Device.GetHandle(), m_Layout, nullptr);
            m_Layout = nullptr;
        }
    }
}