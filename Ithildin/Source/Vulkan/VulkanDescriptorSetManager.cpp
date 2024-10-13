#include "VulkanDescriptorSetManager.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorSets.h"
#include <map>
#include <stdexcept>

namespace Vulkan
{
    VulkanDescriptorSetManager::VulkanDescriptorSetManager(const VulkanDevice& device, const std::vector<VulkanDescriptorBinding>& descriptorBindings, size_t maxSets)
    {
        // Sanity check to avoid binding different resources to the same binding point.
        std::map<uint32_t, VkDescriptorType> bindingTypes;

        for (const VulkanDescriptorBinding& binding : descriptorBindings)
        {
            if (!bindingTypes.insert(std::make_pair(binding.m_BindingIndex, binding.m_Type)).second) // If insertion fails due to a key (our binding) already existing...
            {
                std::invalid_argument("Binding collision.\n");
            }
        }

        m_DescriptorPool.reset(new VulkanDescriptorPool(device, descriptorBindings, maxSets));
        m_DescriptorSetLayout.reset(new VulkanDescriptorSetLayout(device, descriptorBindings));
        m_DescriptorSets.reset(new VulkanDescriptorSets(*m_DescriptorPool, *m_DescriptorSetLayout, bindingTypes, maxSets));
    }

    VulkanDescriptorSetManager::~VulkanDescriptorSetManager()
    {
        m_DescriptorSets.reset();
        m_DescriptorSetLayout.reset();
        m_DescriptorPool.reset();
    }

}
