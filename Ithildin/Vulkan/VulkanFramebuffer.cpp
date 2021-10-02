#include "VulkanFramebuffer.h"
#include "VulkanDevice.h"
#include "VulkanImageView.h"
#include "VulkanRenderPass.h"
#include "VulkanDepthBuffer.h"
#include "VulkanSwapChain.h"
#include <array>

namespace Vulkan
{
    VulkanFramebuffer::VulkanFramebuffer(const VulkanImageView& imageView, const VulkanRenderPass& renderPass)
                                       : m_ImageView(imageView), m_RenderPass(renderPass)
    {
        std::array<VkImageView, 2> attachments =
        {
            imageView.GetHandle(),
            renderPass.GetDepthBuffer().GetImageView().GetHandle()
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass.GetHandle();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = renderPass.GetSwapChain().GetExtent().width;
        framebufferInfo.height = renderPass.GetSwapChain().GetExtent().height;
        framebufferInfo.layers = 1; // Our images are single images, not stereoscopic.
        
        CheckResult(vkCreateFramebuffer(m_ImageView.GetDevice().GetHandle(), &framebufferInfo, nullptr, &m_Framebuffer), "Framebuffer Creation");
    }

    VulkanFramebuffer::VulkanFramebuffer(VulkanFramebuffer&& otherFramebuffer) noexcept
                     : m_ImageView(otherFramebuffer.m_ImageView), m_RenderPass(otherFramebuffer.m_RenderPass), m_Framebuffer(otherFramebuffer.m_Framebuffer)
    {
        otherFramebuffer.m_Framebuffer = nullptr;
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        if (m_Framebuffer != nullptr)
        {
            vkDestroyFramebuffer(m_ImageView.GetDevice().GetHandle(), m_Framebuffer, nullptr);
            m_Framebuffer = nullptr;
        }
    }
}