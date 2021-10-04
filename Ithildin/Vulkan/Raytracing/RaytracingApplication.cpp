#include "RaytracingApplication.h"
#include "../VulkanInstance.h"
#include "../VulkanDebugMessenger.h"
#include "../VulkanSurface.h"
#include "../VulkanDevice.h"
#include "../VulkanImageView.h"
#include "../VulkanCommandPool.h"
#include "../VulkanSwapChain.h"
#include "../VulkanBuffer.h"
#include "../VulkanDepthBuffer.h"
#include "../VulkanFence.h"
#include "../VulkanSemaphore.h"
#include "../VulkanPipelineLayout.h"
#include "../VulkanGraphicsPipeline.h"
#include "../VulkanFramebuffer.h"
#include "../VulkanCommandBuffers.h"
#include "../VulkanRenderPass.h"
#include "VulkanRaytracingCommandList.h"
#include "VulkanRaytracingProperties.h"
#include "VulkanBottomLevelAS.h"
#include "VulkanTopLevelAS.h"
#include "VulkanBottomLevelGeometry.h"
#include "VulkanRaytracingPipeline.h"
#include "Resources/UniformBuffer.h"
#include "Resources/Scene.h"
#include "Resources/Model.h"
#include "Resources/Texture.h"
#include "Core/Window.h"
#include <string>

namespace Vulkan::Raytracing
{
    RaytracingApplication::RaytracingApplication(const WindowSettings& windowSettings, VkPresentModeKHR requestedPresentationMode, bool enabledValidationLayers)
                           : Vulkan::Application(windowSettings, requestedPresentationMode, enabledValidationLayers)
    {
    }

    RaytracingApplication::~RaytracingApplication()
    {

    }

    void RaytracingApplication::SetPhysicalDevice(VkPhysicalDevice physicalDevice, std::vector<const char*>& requiredExtensions,
                                                    VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures)
    {
        // Required extensions.
        requiredExtensions.insert(requiredExtensions.end(),
        {
            VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
            VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
            VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
        });

        VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
        bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
        bufferDeviceAddressFeatures.pNext = nextDeviceFeatures;
        bufferDeviceAddressFeatures.bufferDeviceAddress = true;

        VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures = {};
        indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        indexingFeatures.pNext = &bufferDeviceAddressFeatures;
        indexingFeatures.runtimeDescriptorArray = true;
        indexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;

        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};
        accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        accelerationStructureFeatures.pNext = &indexingFeatures;
        accelerationStructureFeatures.accelerationStructure = true;

        VkPhysicalDeviceRayTracingPipelineFeaturesKHR raytracingFeatures = {};
        raytracingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        raytracingFeatures.pNext = &accelerationStructureFeatures;
        raytracingFeatures.rayTracingPipeline = true;

        Vulkan::Application::SetPhysicalDevice(physicalDevice, requiredExtensions, deviceFeatures, &raytracingFeatures);
    }

    void RaytracingApplication::OnDeviceSet()
    {
        Vulkan::Application::OnDeviceSet();

        m_DeviceCommandList.reset(new VulkanRaytracingCommandList(GetDevice()));
        m_RaytracingProperties.reset(new VulkanRaytracingProperties(GetDevice()));
    }

    void RaytracingApplication::CreateSwapChain()
    {
        Vulkan::Application::CreateSwapChain();

        CreateOutputImage();
    }

    void RaytracingApplication::CreateOutputImage()
    {
        const VkExtent2D extent = GetSwapChain().GetExtent();
        const VkFormat format = GetSwapChain().GetFormat();
        const VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL; // We will always go for optimal tiling.

        m_AccumulationImage.reset(new VulkanImage(GetDevice(), extent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT));
        m_AccumulationImageMemory.reset(new VulkanDeviceMemory(m_AccumulationImage->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
        m_AccumulationImageView.reset(new VulkanImageView(GetDevice(), m_AccumulationImage->GetHandle(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT));

        m_OutputImage.reset(new VulkanImage(GetDevice(), extent, format, tiling, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
        m_OutputImageMemory.reset(new VulkanDeviceMemory(m_OutputImage->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
        m_OutputImageView.reset(new VulkanImageView(GetDevice(), m_OutputImage->GetHandle(), format, VK_IMAGE_ASPECT_COLOR_BIT));

        const VulkanDebugUtilities& debugUtilities = GetDevice().GetDebugUtilities();

        debugUtilities.SetObjectName(m_AccumulationImage->GetHandle(), "Accumulation Image");
        debugUtilities.SetObjectName(m_AccumulationImageMemory->GetHandle(), "Accumulation Image Memory");
        debugUtilities.SetObjectName(m_AccumulationImageView->GetHandle(), "Accumulation Image View");

        debugUtilities.SetObjectName(m_OutputImage->GetHandle(), "Output Image");
        debugUtilities.SetObjectName(m_OutputImageMemory->GetHandle(), "Output Image Memory");
        debugUtilities.SetObjectName(m_OutputImageView->GetHandle(), "Output Image View");
    }
}