#include "VulkanDepthBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"
#include "VulkanDeviceMemory.h"
#include "VulkanDebugUtilities.h"
#include <stdexcept>

namespace Raytracing
{
    namespace DepthBufferUtilities
    {
        VkFormat FindSupportedFormat(const VulkanDevice& device, const std::vector<VkFormat>& formatCandidates, const VkImageTiling tiling, const VkFormatFeatureFlags features)
        {
            for (VkFormat format : formatCandidates)
            {
                VkFormatProperties formatProperties;
                vkGetPhysicalDeviceFormatProperties(device.GetPhysicalDevice(), format, &formatProperties);

                // Queries which VkFormatFeatureFlags our requested format supports, split into linear and optimal tiling.
                if (tiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & features))
                {
                    return format;
                }

                if (tiling == VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & features))
                {
                    return format;
                }
            }

            std::runtime_error("Failed to find the supported format for depth image buffers.\n");
        }

        VkFormat FindDepthFormat(const VulkanDevice& device)
        {
            return FindSupportedFormat(device,
                                     { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                                       VK_IMAGE_TILING_OPTIMAL,
                                       VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        }
    }

    VulkanDepthBuffer::VulkanDepthBuffer(VulkanCommandPool& commandPool, VkExtent2D extent) : m_Format(DepthBufferUtilities::FindDepthFormat(commandPool.GetDevice()))
    {
        const VulkanDevice& device = commandPool.GetDevice();

        m_Image.reset(new VulkanImage(device, extent, m_Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)); // Specifies that our image will be used as a depth stencil attachment.
        m_ImageMemory.reset(new VulkanDeviceMemory(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))); // Best for device (GPU) access. 
        m_ImageView.reset(new VulkanImageView(device, m_Image->GetHandle(), m_Format, VK_IMAGE_ASPECT_DEPTH_BIT)); // We specify that the depth aspect of our image will be included in the view.
        
        m_Image->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL); // Transition to a layout most efficient to be used as a depth stencil attachment.

        const VulkanDebugUtilities& debugUtilities = device.GetDebugUtilities();

        debugUtilities.SetObjectName(m_Image->GetHandle(), "Depth Buffer Image");
        debugUtilities.SetObjectName(m_ImageMemory->GetHandle(), "Depth Buffer Image Memory");
        debugUtilities.SetObjectName(m_ImageView->GetHandle(), "Depth Buffer Image View");
    }

    VulkanDepthBuffer::~VulkanDepthBuffer()
    {
        m_ImageView.reset();
        m_Image.reset();
        m_ImageMemory.reset(); // Release memory after the bound image has been destroyed.
    }
}