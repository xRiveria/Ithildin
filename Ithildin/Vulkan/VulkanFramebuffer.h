#pragma once
#include "Core/Core.h"

namespace Vulkan
{
    class VulkanImageView;
    class VulkanRenderPass;

    class VulkanFramebuffer final
    {
    public:
        explicit VulkanFramebuffer(const VulkanImageView& imageView, const VulkanRenderPass& renderPass);
        VulkanFramebuffer(VulkanFramebuffer&& otherFramebuffer) noexcept;
        ~VulkanFramebuffer();

        const VulkanImageView& GetImageView() const { return m_ImageView; }
        const VulkanRenderPass& GetRenderPass() const { return m_RenderPass; }

    private:
        const VulkanImageView& m_ImageView;
        const VulkanRenderPass& m_RenderPass;

        VULKAN_HANDLE(VkFramebuffer, m_Framebuffer)
    };
}
