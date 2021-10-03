#pragma once
#include "Vulkan/Application.h"

namespace Vulkan
{
    class VulkanCommandBuffers;
    class VulkanBuffer;
    class VulkanDeviceMemory;
    class VulkanImage;
    class VulkanImageView;
}

namespace Vulkan::Raytracing
{
    class RaytracingApplication : public Vulkan::Application
    {
    public:
        RaytracingApplication(const WindowSettings& windowSettings, VkPresentModeKHR requestedPresentationMode, bool enabledValidationLayers);
        ~RaytracingApplication();
        
        virtual void SetPhysicalDevice(VkPhysicalDevice physicalDevice, std::vector<const char*>& requiredExtensions,
                                       VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures) override;
        virtual void OnDeviceSet() override;
        virtual void CreateSwapChain() override;
        virtual void DeleteSwapChain() override;
        virtual void Render(VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

        // Raytracing
        void CreateAccelerationStructures();
        void DeleteAccelerationStructures();

    private:
        void CreateBottomLevelStructures(VkCommandBuffer commandBuffer);
        void CreateTopLevelStructures(VkCommandBuffer commandBuffer);
        void CreateOutputImage();

    private:
        // Raytracing
        std::unique_ptr<class VulkanDeviceProcedures> m_DeviceProcedures;
        std::unique_ptr<class VulkanRaytracingProperties> m_RaytracingProperties;
        std::vector<class VulkanBottomLevelAccelerationStructure> m_BottomAccelerationStructure;
        std::vector<class VulkanTopLevelAccelerationStructure> m_TopLevelAccelerationStructure;
        std::unique_ptr<class VulkanRaytracingPipeline> m_RaytracingPipeline;
        std::unique_ptr<class VulkanShaderBindingTable> m_ShaderBindingTable;

        std::unique_ptr<VulkanImage> m_AccumulationImage;
        std::unique_ptr<VulkanDeviceMemory> m_AccumulationImageMemory;
        std::unique_ptr<VulkanImageView> m_AccumulationImageView;

        std::unique_ptr<VulkanImage> m_OutputImage;
        std::unique_ptr<VulkanDeviceMemory> m_OutputImageMemory;
        std::unique_ptr<VulkanImageView> m_OutputImageView;
    };
}