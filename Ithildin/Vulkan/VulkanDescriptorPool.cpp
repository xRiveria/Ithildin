#include "VulkanDescriptorPool.h"
#include "VulkanDevice.h"

namespace Vulkan
{
    VulkanDescriptorPool::VulkanDescriptorPool(const VulkanDevice& device, const std::vector<VulkanDescriptorBinding>& descriptorBindings, size_t maxSets) :
                                               m_Device(device)
    {
        std::vector<VkDescriptorPoolSize> poolSizes;

        for (const VulkanDescriptorBinding& descriptorBinding : descriptorBindings)
        {
            poolSizes.push_back(VkDescriptorPoolSize{ descriptorBinding.m_Type, static_cast<uint32_t>(descriptorBinding.m_DescriptorCount * maxSets) });
        }

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(maxSets);

        CheckResult(vkCreateDescriptorPool(m_Device.GetHandle(), &poolInfo, nullptr, &m_DescriptorPool), "Descriptor Pool Creation");
    }

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        if (m_DescriptorPool != nullptr)
        {
            vkDestroyDescriptorPool(m_Device.GetHandle(), m_DescriptorPool, nullptr);
            m_DescriptorPool = nullptr;
        }
    }
}