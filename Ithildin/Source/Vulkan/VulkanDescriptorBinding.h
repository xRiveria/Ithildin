#pragma once
#include "Core/Core.h"

namespace Vulkan
{
    struct VulkanDescriptorBinding
    {
        uint32_t m_BindingIndex;         // Slot to which the descriptor will be bound, corresponding to the layout index in the shader.
        uint32_t m_DescriptorCount;      // Number of descriptors to bind, accessed in a shader as an array except if the type is VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT.
        VkDescriptorType m_Type;         // Type of the bound descriptor(s). https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDescriptorType.html
        VkShaderStageFlags m_StageFlags; // Shader stage at which the bound resources will be avaliable.
    };
}