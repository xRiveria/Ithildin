#pragma once
#include "Core/Core.h"
#include <memory>
#include <vector>

namespace Resources
{
    class Scene;
    class UniformBuffer;
}

namespace Vulkan
{
    class VulkanDescriptorSetManager;
    class VulkanImageView;
    class VulkanPipelineLayout;
    class VulkanSwapChain;
}

namespace Vulkan::Raytracing
{
    class VulkanRaytracingCommandList;
    class VulkanTopLevelAS;

    class VulkanRaytracingPipeline final
    {
    public:
        VulkanRaytracingPipeline(const VulkanRaytracingCommandList& commandList, const VulkanSwapChain& swapChain, const VulkanTopLevelAS& accelerationStructure,
                                 const VulkanImageView& accumulationImageView, const VulkanImageView& outputImageView, 
                                 const std::vector<Resources::UniformBuffer>& uniformBuffers, const Resources::Scene& scene);
        ~VulkanRaytracingPipeline();

        uint32_t GetRayGenerationShaderIndex() const { return m_RayGenerationShaderIndex; }
        uint32_t GetMissShaderIndex() const { return m_MissShaderIndex; }
        uint32_t GetTriangleHitGroupIndex() const { return m_TriangleHitGroupIndex; }
        uint32_t GetProceduralHitGroupIndex() const { return m_ProceduralHitGroupIndex; }

        VkDescriptorSet GetDescriptorSet(uint32_t index) const;
        const VulkanPipelineLayout& GetPipelineLayout() const { return *m_PipelineLayout; }

    private:
        const VulkanSwapChain& m_SwapChain;

        std::unique_ptr<VulkanDescriptorSetManager> m_DescriptorSetManager;
        std::unique_ptr<VulkanPipelineLayout> m_PipelineLayout;

        uint32_t m_RayGenerationShaderIndex;
        uint32_t m_MissShaderIndex;
        uint32_t m_TriangleHitGroupIndex;
        uint32_t m_ProceduralHitGroupIndex;

        VULKAN_HANDLE(VkPipeline, m_Pipeline)
    };
}