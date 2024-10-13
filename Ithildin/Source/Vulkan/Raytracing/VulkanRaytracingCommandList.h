#pragma once
#include "Core/Core.h"
#include <functional>

namespace Vulkan
{
    class VulkanDevice;

    namespace Raytracing
    {
        class VulkanRaytracingCommandList final
        {
        public:
            explicit VulkanRaytracingCommandList(const VulkanDevice& device);
            ~VulkanRaytracingCommandList();

            const VulkanDevice& GetDevice() const { return m_Device; }

            // Acceleration Structures
            const std::function<VkResult(VkDevice device, 
                                         const VkAccelerationStructureCreateInfoKHR* pCreateInfo, 
                                         const VkAllocationCallbacks* pAllocator,
                                         VkAccelerationStructureKHR* pAccelerationStructure)> 
                                         vkCreateAccelerationStructureKHR;

            const std::function<void(VkDevice device, 
                                         const VkAccelerationStructureKHR accelerationStructure, 
                                         const VkAllocationCallbacks* pAllocator)> 
                                         vkDestroyAccelerationStructureKHR;

            const std::function<void(VkDevice device, 
                                         VkAccelerationStructureBuildTypeKHR buildType, 
                                         const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo,
                                         const uint32_t* pMaxPrimitiveCounts, 
                                         VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo)> 
                                         vkGetAccelerationStructureBuildSizesKHR;

            const std::function<void(VkCommandBuffer commandBuffer,
                                     uint32_t infoCount,
                                     const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
                                     const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos)> 
                                     vkCmdBuildAccelerationStructuresKHR;

            const std::function<void(VkCommandBuffer commandBuffer, 
                                     const VkCopyAccelerationStructureInfoKHR* pInfo)> 
                                     vkCmdCopyAccelerationStructureKHR;

            const std::function<void(VkCommandBuffer commandBuffer,
                                     const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
                                     const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable,
                                     const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
                                     const VkStridedDeviceAddressRegionKHR* pCallbackShaderBindingTable,
                                     uint32_t width,
                                     uint32_t height,
                                     uint32_t depth)> 
                                     vkCmdTraceRaysKHR;

            const std::function<VkResult(VkDevice device, 
                                         VkDeferredOperationKHR deferredOperation,
                                         VkPipelineCache pipelineCache,
                                         uint32_t createInfoCount,
                                         const VkRayTracingPipelineCreateInfoKHR* pCreateInfos,
                                         const VkAllocationCallbacks* pAllocator,
                                         VkPipeline* pPipelines)>
                                         vkCreateRayTracingPipelinesKHR;

            const std::function<VkResult(VkDevice device,
                                VkPipeline pipeline,
                                uint32_t firstGroup,
                                uint32_t groupCount,
                                size_t dataSize,
                                void* pData)> 
                                vkGetRayTracingShaderGroupHandlesKHR;

            const std::function<VkDeviceAddress(VkDevice device, 
                                                const VkAccelerationStructureDeviceAddressInfoKHR* pInfo)> 
                                                vkGetAccelerationStructureDeviceAddressKHR;

            const std::function<void(VkCommandBuffer commandBuffer,
                                     uint32_t accelerationStructureCount,
                                     const VkAccelerationStructureKHR* pAccelerationStructures,
                                     VkQueryType queryType,
                                     VkQueryPool queryPool,
                                     uint32_t firstQuerty)> 
                                     vkCmdWriteAccelerationStructuresPropertiesKHR;
                                     

        private:
            const VulkanDevice& m_Device;
        };
    }
}