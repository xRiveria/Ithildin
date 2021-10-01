#pragma once
#include "Core/Core.h"

namespace Raytracing
{
    class VulkanDevice;

    struct SamplerConfiguration
    {
        VkFilter m_MagificationFilter = VK_FILTER_LINEAR;
        VkFilter m_MinificationFilter = VK_FILTER_LINEAR;
        VkSamplerAddressMode m_AddressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        VkSamplerAddressMode m_AddressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        VkSamplerAddressMode m_AddressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        bool m_IsAnistropyEnabled = true;
        float m_MaxAnisotropy = 16;             // Limits the amount of texel samples that can be used to calculate the final color. Increase the quality textures are viewed at an angle.
        VkBorderColor m_BorderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        bool m_UnnormalizedCoordinates = false; // Specifies which coordinate system you wish to use to address texels in the image. For true, texels are addressed using the [0, texWidth/texHeight] range. Else, texels use the [0, 1] range.
        bool m_ComparisonEnabled = false;       // If a comparison filter is enabled, then texels will first be compared to a value, and the result of that operation will be used in filtering operations. This is mainly used for percentage closer filtering.
        VkCompareOp m_ComparisonOperation = VK_COMPARE_OP_ALWAYS;
        VkSamplerMipmapMode m_MipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; 
        float m_MipLODBias = 0.0f;
        float m_MipMinimum = 0.0f;
        float m_MipMaximum = 0.0f;
    };

    class VulkanSampler final
    {
    public:
        VulkanSampler(const VulkanDevice& device, const SamplerConfiguration& configuration);
        ~VulkanSampler();

        const VulkanDevice& GetDevice() const { return m_Device; }

    private:
        const VulkanDevice& m_Device;
        VULKAN_HANDLE(VkSampler, m_Sampler)
    };
}