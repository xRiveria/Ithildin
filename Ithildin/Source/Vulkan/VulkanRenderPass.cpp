#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"
#include "VulkanDepthBuffer.h"
#include "VulkanDevice.h"
#include <array>

namespace Vulkan
{
    VulkanRenderPass::VulkanRenderPass(const VulkanSwapChain& swapChain, const VulkanDepthBuffer& depthBuffer, VkAttachmentLoadOp colorBufferLoadOperation, VkAttachmentLoadOp depthBufferLoadOperation)
                                      : m_SwapChain(swapChain), m_DepthBuffer(depthBuffer)
    {
        // Describe attachments that are part of our render pass.
        VkAttachmentDescription colorAttachmentInfo = {};
        colorAttachmentInfo.format = swapChain.GetFormat();
        colorAttachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentInfo.loadOp = colorBufferLoadOperation;      // Remember that loadOp and storeOp refer to color and depth data.
        colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // We want to see our rendering on screen. Hence, we will store results of the operations.
        colorAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        // Set as undefined if our load operation is a buffer clear since its existing contents don't matter due to the clear.
        colorAttachmentInfo.initialLayout = colorBufferLoadOperation == VK_ATTACHMENT_LOAD_OP_CLEAR ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        colorAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Set to presentation layout.

        VkAttachmentDescription depthAttachmentInfo = {};
        depthAttachmentInfo.format = depthBuffer.GetFormat();
        depthAttachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachmentInfo.loadOp = depthBufferLoadOperation;
        depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentInfo.initialLayout = depthBufferLoadOperation == VK_ATTACHMENT_LOAD_OP_CLEAR ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // Color
        VkAttachmentReference colorAttachmentReferenceInfo = {};
        colorAttachmentReferenceInfo.attachment = 0; // Our VkAttachmentDescription for the color attachment is index 0.
        colorAttachmentReferenceInfo.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // Depth
        VkAttachmentReference depthAttachmentReferenceInfo = {};
        depthAttachmentReferenceInfo.attachment = 1;
        depthAttachmentReferenceInfo.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // Subpass
        VkSubpassDescription subpassDescriptionInfo = {};
        subpassDescriptionInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescriptionInfo.colorAttachmentCount = 1;
        subpassDescriptionInfo.pColorAttachments = &colorAttachmentReferenceInfo;
        subpassDescriptionInfo.pDepthStencilAttachment = &depthAttachmentReferenceInfo;

        // Subpass Dependency 
        VkSubpassDependency subpassDependencyInfo = {};
        subpassDependencyInfo.srcSubpass = VK_SUBPASS_EXTERNAL; // The implicit subpass before the render pass.
        subpassDependencyInfo.dstSubpass = 0; // Our subpass at index 0.
        subpassDependencyInfo.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Operations to wait on. We will wait for the swapchain to finish reading from the image before we can access it. Hence, we will wait on the color attachment output stage.
        subpassDependencyInfo.srcAccessMask = 0;
        subpassDependencyInfo.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // We will prevent the transition below from happening until its necessary and allowed (after our swapchain has finished reading from the image): when we want to start writing colors to it.
        subpassDependencyInfo.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachmentInfo, depthAttachmentInfo };

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassDescriptionInfo;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &subpassDependencyInfo;

        CheckResult(vkCreateRenderPass(m_SwapChain.GetDevice().GetHandle(), &renderPassInfo, nullptr, &m_RenderPass), "Render Pass Creation");
    }

    VulkanRenderPass::~VulkanRenderPass()
    {
        if (m_RenderPass != nullptr)
        {
            vkDestroyRenderPass(m_SwapChain.GetDevice().GetHandle(), m_RenderPass, nullptr);
            m_RenderPass = nullptr;
        }
    }
}