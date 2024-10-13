#pragma once
#include "Core/Core.h"
#include <string>
#include <vector>

namespace Vulkan
{
    class VulkanDevice;

    class VulkanShaderModule final
    {
    public:
        VulkanShaderModule(const VulkanDevice& device, const std::string& filePath);
        VulkanShaderModule(const VulkanDevice& device, const std::vector<char>& shaderCode);
        ~VulkanShaderModule();

        const VulkanDevice& GetDevice() const { return m_Device; }
        VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderStageFlagBits stage) const;

    private:
        static std::vector<char> ReadFile(const std::string& filePath);

    private:
        const VulkanDevice& m_Device;
        VULKAN_HANDLE(VkShaderModule, m_ShaderModule)
    };
}