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


}