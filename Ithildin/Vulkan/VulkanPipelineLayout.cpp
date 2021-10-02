#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorSetLayout.h"

namespace Vulkan
{
    VulkanPipelineLayout::VulkanPipelineLayout(const VulkanDevice& device, const VulkanDescriptorSetLayout& descriptorSetLayout) : m_Device(device)
    {
        VkDescriptorSetLayout descriptorSetLayouts[] = { descriptorSetLayout.GetHandle() };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        CheckResult(vkCreatePipelineLayout(device.GetHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout), "Pipeline Layout Creation");
    }

    VulkanPipelineLayout::~VulkanPipelineLayout()
    {
        if (m_PipelineLayout != nullptr)
        {
            vkDestroyPipelineLayout(m_Device.GetHandle(), m_PipelineLayout, nullptr);
            m_PipelineLayout = nullptr;
        }
    }
}