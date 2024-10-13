#include "VulkanImageView.h"
#include "VulkanDevice.h"

namespace Vulkan
{
    VulkanImageView::VulkanImageView(const VulkanDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
                                   : m_Device(device), m_VulkanImage(image), m_VulkanFormat(format)
    {
        VkImageViewCreateInfo imageViewCreationInfo = {};
        imageViewCreationInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreationInfo.image = image;
        imageViewCreationInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreationInfo.format = format;
        imageViewCreationInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreationInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreationInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreationInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreationInfo.subresourceRange.aspectMask = aspectFlags;
        imageViewCreationInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreationInfo.subresourceRange.levelCount = 1;
        imageViewCreationInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreationInfo.subresourceRange.layerCount = 1;

        CheckResult(vkCreateImageView(m_Device.GetHandle(), &imageViewCreationInfo, nullptr, &m_ImageView), "Image View Creation");
    }

    VulkanImageView::~VulkanImageView()
    {
        if (m_ImageView != nullptr)
        {
            vkDestroyImageView(m_Device.GetHandle(), m_ImageView, nullptr);
            m_ImageView = nullptr;
        }
    }
}