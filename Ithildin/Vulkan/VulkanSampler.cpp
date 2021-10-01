#include "VulkanSampler.h"
#include "VulkanDevice.h"

namespace Raytracing
{
    VulkanSampler::VulkanSampler(const VulkanDevice& device, const SamplerConfiguration& configuration) : m_Device(device)
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = configuration.m_MagificationFilter;
        samplerInfo.minFilter = configuration.m_MinificationFilter;
        samplerInfo.addressModeU = configuration.m_AddressModeU;
        samplerInfo.addressModeV = configuration.m_AddressModeV;
        samplerInfo.addressModeW = configuration.m_AddressModeW;
        samplerInfo.anisotropyEnable = configuration.m_IsAnistropyEnabled;
        samplerInfo.maxAnisotropy = configuration.m_MaxAnisotropy;
        samplerInfo.borderColor = configuration.m_BorderColor;
        samplerInfo.unnormalizedCoordinates = configuration.m_UnnormalizedCoordinates;
        samplerInfo.compareEnable = configuration.m_ComparisonEnabled;
        samplerInfo.compareOp = configuration.m_ComparisonOperation;
        samplerInfo.mipmapMode = configuration.m_MipmapMode;
        samplerInfo.mipLodBias = configuration.m_MipLODBias;
        samplerInfo.minLod = configuration.m_MipMinimum;
        samplerInfo.maxLod = configuration.m_MipMaximum;

        CheckResult(vkCreateSampler(device.GetHandle(), &samplerInfo, nullptr, &m_Sampler), "Sampler Creation Success");
    }

    VulkanSampler::~VulkanSampler()
    {
        if (m_Sampler != nullptr)
        {
            vkDestroySampler(m_Device.GetHandle(), m_Sampler, nullptr);
            m_Sampler = nullptr;
        }
    }
}