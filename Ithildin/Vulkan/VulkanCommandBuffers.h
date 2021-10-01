#pragma once
#include "../Core/Core.h"
#include <vector>

namespace Raytracing
{
    class VulkanCommandPool;

    class VulkanCommandBuffers final
    {
    public:
        VulkanCommandBuffers(VulkanCommandPool& commandPool, uint32_t size);
        ~VulkanCommandBuffers();

        uint32_t GetSize() const { return static_cast<uint32_t>(m_CommandBuffers.size()); }
        VkCommandBuffer& operator[](size_t i) { return m_CommandBuffers[i]; }

        VkCommandBuffer BeginRecording(size_t i);
        void EndRecording(size_t i);

    private:
        const VulkanCommandPool& m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;
    };
}