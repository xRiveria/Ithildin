#pragma once
#include <vector>
#include "../Core/Core.h"
#include "VulkanDebugUtilities.h"

namespace Raytracing
{
    class VulkanSurface;

    class VulkanDevice final
    {
    public:
        VulkanDevice(VkPhysicalDevice physicalDevice, const VulkanSurface& surface, const std::vector<const char*>& requiredExtensions,
                     const VkPhysicalDeviceFeatures& deviceFeatures, const void* nextDeviceFeatures);
        ~VulkanDevice();

        VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
        const VulkanSurface& GetSurface() const { return m_Surface; }
        const VulkanDebugUtilities& GetDebugUtilities() const { return m_DebugUtilities; }

        uint32_t GetGraphicsQueueFamilyIndex() const { return m_QueueGraphicsFamilyIndex; }
        uint32_t GetComputeQueueFamilyIndex() const { return m_QueueComputeFamilyIndex; }
        uint32_t GetPresentQueueFamilyIndex() const { return m_QueuePresentFamilyIndex; }
        uint32_t GetTransferQueueFamilyIndex() const { return m_QueueTransferFamilyIndex; }

        VkQueue GetGraphicsQueue() const { return m_QueueGraphics; }
        VkQueue GetComputeQueue() const { return m_QueueCompute; }
        VkQueue GetPresentQueue() const { return m_QueuePresent; }
        VkQueue GetTransferQueue() const { return m_QueueTransfer; }

        void WaitIdle() const;

    private:
        void QueryRequiredExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions) const;

    private:
        const VkPhysicalDevice m_PhysicalDevice;
        const VulkanSurface& m_Surface;

        uint32_t m_QueueGraphicsFamilyIndex = 0;
        uint32_t m_QueueComputeFamilyIndex = 0;
        uint32_t m_QueuePresentFamilyIndex = 0;
        uint32_t m_QueueTransferFamilyIndex = 0;

        VkQueue m_QueueGraphics = nullptr;
        VkQueue m_QueueCompute = nullptr;
        VkQueue m_QueuePresent = nullptr;
        VkQueue m_QueueTransfer = nullptr;

        VulkanDebugUtilities m_DebugUtilities;
        VULKAN_HANDLE(VkDevice, m_Device)
    };
}