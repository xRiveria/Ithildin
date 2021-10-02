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
    class VulkanDepthBuffer;
    class VulkanPipelineLayout;
    class VulkanRenderPass;
    class VulkanSwapChain;
    class VulkanDescriptorSetManager;

    class VulkanGraphicsPipeline final
    {
    public:
        VulkanGraphicsPipeline(const VulkanSwapChain& swapChain, const VulkanDepthBuffer& depthBuffer,
                               const std::vector<Resources::UniformBuffer>& uniformBuffers, const Resources::Scene& scene, bool isWireFrame);
        ~VulkanGraphicsPipeline();

        VkDescriptorSet GetDescriptorSet(uint32_t index) const;
        const VulkanRenderPass& GetRenderPass() const { return *m_RenderPass; }
        const VulkanPipelineLayout& GetPipelineLayout() const { return *m_PipelineLayout; }
        bool IsWireFrame() const { return m_IsWireFrame; }

    private:
        const VulkanSwapChain& m_SwapChain;
        const bool m_IsWireFrame;

        std::unique_ptr<VulkanDescriptorSetManager> m_DescriptorSetManager;
        std::unique_ptr<VulkanPipelineLayout> m_PipelineLayout;
        std::unique_ptr<VulkanRenderPass> m_RenderPass;

        VULKAN_HANDLE(VkPipeline, m_Pipeline)
    };
}