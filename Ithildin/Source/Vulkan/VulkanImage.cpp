#include "VulkanImage.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanDepthBuffer.h"
#include "SingleTimeCommands.h"
#include <stdexcept>

namespace Vulkan
{
    VulkanImage::VulkanImage(const VulkanDevice& device, VkExtent2D extent, VkFormat format)
                           : VulkanImage(device, extent, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT) // Allows for sampling by a shader.
    {
    }

    VulkanImage::VulkanImage(const VulkanDevice& device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags)
                           : m_Device(device), m_Extent(extent), m_Format(format), m_ImageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
    {
        VkImageCreateInfo imageCreationInfo = {};
        imageCreationInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreationInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreationInfo.extent.width = extent.width;
        imageCreationInfo.extent.height = extent.height;
        imageCreationInfo.extent.depth = 1;
        imageCreationInfo.mipLevels = 1;
        imageCreationInfo.arrayLayers = 1;
        imageCreationInfo.format = format;
        imageCreationInfo.tiling = tiling;
        imageCreationInfo.initialLayout = m_ImageLayout; // Starting as Undefined is possible. However, data is not guarenteed to be preserved.
        imageCreationInfo.usage = usageFlags;
        imageCreationInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreationInfo.samples = VK_SAMPLE_COUNT_1_BIT; // 1 sample per pixel.
        imageCreationInfo.flags = 0; // Optional

        CheckResult(vkCreateImage(m_Device.GetHandle(), &imageCreationInfo, nullptr, &m_Image), "Image Creation");
    }

    VulkanImage::VulkanImage(VulkanImage&& otherImage) noexcept : m_Device(otherImage.m_Device), m_Extent(otherImage.m_Extent), m_Format(otherImage.m_Format),
                                                                  m_ImageLayout(otherImage.m_ImageLayout), m_Image(otherImage.m_Image)
    {
        otherImage.m_Image = nullptr;
    }

    VulkanImage::~VulkanImage()
    {
        if (m_Image != nullptr)
        {
            vkDestroyImage(m_Device.GetHandle(), m_Image, nullptr);
            m_Image = nullptr;
        }
    }

    VulkanDeviceMemory VulkanImage::AllocateMemory(VkMemoryPropertyFlags propertyFlags) const
    {
        const VkMemoryRequirements memoryRequirements = GetMemoryRequirements();
        VulkanDeviceMemory memory(m_Device, memoryRequirements.size, memoryRequirements.memoryTypeBits, 0, propertyFlags);

        CheckResult(vkBindImageMemory(m_Device.GetHandle(), m_Image, memory.GetHandle(), 0), "Bind Memory to Image");

        return memory;
    }

    VkMemoryRequirements VulkanImage::GetMemoryRequirements() const
    {
        VkMemoryRequirements memoryRequirements;
        // memoryTypeBits only sets bits if the physical device supports it.
        vkGetImageMemoryRequirements(m_Device.GetHandle(), m_Image, &memoryRequirements);

        return memoryRequirements;
    }

    void VulkanImage::TransitionImageLayout(VulkanCommandPool& commandPool, VkImageLayout newLayout)
    {
        SingleTimeCommands::Submit(commandPool, [&](VkCommandBuffer commandBuffer)
        {
            VkImageMemoryBarrier barrierInfo = {};
            barrierInfo.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrierInfo.oldLayout = m_ImageLayout;
            barrierInfo.newLayout = newLayout;
            // We may also transfer ownership of the image between queues. We will use VK_QUEUE_FAMILY_IGNORED to specify that no transfers are taking place.
            barrierInfo.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrierInfo.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrierInfo.image = m_Image;
            barrierInfo.subresourceRange.baseMipLevel = 0;
            barrierInfo.subresourceRange.levelCount = 1;
            barrierInfo.subresourceRange.baseArrayLayer = 0;
            barrierInfo.subresourceRange.layerCount = 1;

            if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            {
                barrierInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

                if (VulkanDepthBuffer::HasStencilComponent(m_Format)) // If the forma has a stencil component, specify the stencil bit aspect.
                {
                    barrierInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                }
            }
            else
            {
                barrierInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            }

            VkPipelineStageFlags sourceStage;      // The pipeline stage the operations will occur before the barrer.
            VkPipelineStageFlags destinationStage; // The pipeline stage in which operations will wait on the barrier.

            if (m_ImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            {
                barrierInfo.srcAccessMask = 0;
                barrierInfo.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; 
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            else if (m_ImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            {
                barrierInfo.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrierInfo.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            else if (m_ImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            {
                barrierInfo.srcAccessMask = 0;
                barrierInfo.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            }
            else
            {
                std::invalid_argument("Unsupported Layout Transition.\n");
            }

            // Defines a memory dependency between commands that were submitted before and after this command is submitted.
            vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrierInfo);
        });

        m_ImageLayout = newLayout;
    }

    void VulkanImage::CopyFrom(VulkanCommandPool& commandPool, const VulkanBuffer& buffer)
    {
        SingleTimeCommands::Submit(commandPool, [&](VkCommandBuffer commandBuffer)
        {
            VkBufferImageCopy regionDescription = {};
            regionDescription.bufferOffset = 0;
            regionDescription.bufferRowLength = 0;
            regionDescription.bufferImageHeight = 0;
            regionDescription.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            regionDescription.imageSubresource.mipLevel = 0;
            regionDescription.imageSubresource.baseArrayLayer = 0;
            regionDescription.imageSubresource.layerCount = 1;
            regionDescription.imageOffset = { 0, 0, 0 };
            regionDescription.imageExtent = { m_Extent.width, m_Extent.height, 1 };

            vkCmdCopyBufferToImage(commandBuffer, buffer.GetHandle(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &regionDescription);
        });
    }
}