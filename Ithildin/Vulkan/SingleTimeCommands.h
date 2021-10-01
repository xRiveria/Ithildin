#pragma once
#include "../Core/Core.h"
#include <functional>
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffers.h"

namespace Raytracing
{
    class SingleTimeCommands final
    {
    public:
        static void Submit(VulkanCommandPool& commandPool, const std::function<void(VkCommandBuffer commandBuffer)>& action)
        {
            VulkanCommandBuffers commandBuffers(commandPool, 1); // Allocate

            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffers[0], &beginInfo);

            action(commandBuffers[0]);

            vkEndCommandBuffer(commandBuffers[0]);

            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers[0];

            const VkQueue graphicsQueue = commandPool.GetDevice().GetGraphicsQueue();

            // Submits our command onto the graphics queue.
            vkQueueSubmit(graphicsQueue, 1, &submitInfo, nullptr);
            vkQueueWaitIdle(graphicsQueue); // Ensures the execution is complete.
        }
    };
}