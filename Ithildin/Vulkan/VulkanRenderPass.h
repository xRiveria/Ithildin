#pragma once
#include "Core/Core.h"

namespace Vulkan
{
    class VulkanDepthBuffer;
    class VulkanSwapChain;

    class VulkanRenderPass final
    {
    public:
        VulkanRenderPass(const VulkanSwapChain& swapChain, const VulkanDepthBuffer& depthBuffer, 
                         VkAttachmentLoadOp colorBufferLoadOperation, VkAttachmentLoadOp depthBufferLoadOperation);
        ~VulkanRenderPass();

        const VulkanDepthBuffer& GetDepthBuffer() const { return m_DepthBuffer; }
        const VulkanSwapChain& GetSwapChain() const { return m_SwapChain; }

    private:
        const VulkanSwapChain& m_SwapChain;
        const VulkanDepthBuffer& m_DepthBuffer;

        VULKAN_HANDLE(VkRenderPass, m_RenderPass)
    };
}