#pragma once
#include "Core/Core.h"

namespace Vulkan
{
    class VulkanImageMemoryBarrier final
    {
    public:
        static void Insert(const VkCommandBuffer commandBuffer, const VkImage image, const VkImageSubresourceRange subresourceRange, const VkAccessFlags sourceAccessMask,
                           const VkAccessFlags destinationAccessMask, const VkImageLayout oldLayout, const VkImageLayout newLayout)
        {
            VkImageMemoryBarrier memoryBarrier = {};
            memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            memoryBarrier.pNext = nullptr;
            memoryBarrier.srcAccessMask = sourceAccessMask;
            memoryBarrier.dstAccessMask = destinationAccessMask;
            memoryBarrier.oldLayout = oldLayout;
            memoryBarrier.newLayout = newLayout;
            memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memoryBarrier.image = image;
            memoryBarrier.subresourceRange = subresourceRange;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
        }
    };
}