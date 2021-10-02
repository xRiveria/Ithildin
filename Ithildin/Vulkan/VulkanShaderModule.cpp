#include "VulkanShaderModule.h"
#include "VulkanDevice.h"
#include <fstream>

namespace Vulkan
{
    VulkanShaderModule::VulkanShaderModule(const VulkanDevice& device, const std::string& filePath) : VulkanShaderModule(device, ReadFile(filePath))
    {

    }

    VulkanShaderModule::VulkanShaderModule(const VulkanDevice& device, const std::vector<char>& shaderCode) : m_Device(device)
    {
        VkShaderModuleCreateInfo moduleCreationInfo = {};
        moduleCreationInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreationInfo.codeSize = shaderCode.size();
        moduleCreationInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

        CheckResult(vkCreateShaderModule(device.GetHandle(), &moduleCreationInfo, nullptr, &m_ShaderModule), "Shader Module Creation");
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        if (m_ShaderModule != nullptr)
        {
            vkDestroyShaderModule(m_Device.GetHandle(), m_ShaderModule, nullptr);
            m_ShaderModule = nullptr;
        }
    }

    VkPipelineShaderStageCreateInfo VulkanShaderModule::CreateShaderStage(VkShaderStageFlagBits stage) const
    {
        VkPipelineShaderStageCreateInfo stageCreationInfo = {};
        stageCreationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageCreationInfo.stage = stage;
        stageCreationInfo.module = m_ShaderModule;
        stageCreationInfo.pName = "main"; // Entry Point

        return stageCreationInfo;
    }

    std::vector<char> VulkanShaderModule::ReadFile(const std::string& filePath)
    {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary); // Read in binary and skip to end of the file after opening.

        if (!file.is_open())
        {
            std::runtime_error("Failed to open shader file for reading: " + filePath);
        }

        const size_t fileSize = static_cast<size_t>(file.tellg()); // Read file size as we're currently on the last character.
        std::vector<char> buffer(fileSize);

        file.seekg(0); // Return to the first character in the file.
        file.read(buffer.data(), fileSize); // Read.
        file.close();

        return buffer;
    }
}