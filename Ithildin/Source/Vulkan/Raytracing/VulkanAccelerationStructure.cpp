#include "VulkanAccelerationStructure.h"
#include "VulkanRaytracingCommandList.h"
#include "VulkanRaytracingProperties.h"
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanDevice.h"

namespace Vulkan::Raytracing
{
    VulkanAccelerationStructure::VulkanAccelerationStructure(const VulkanRaytracingCommandList& commandList, const VulkanRaytracingProperties& raytracingProperties)
                               : m_CommandList(commandList), m_BuildFlags(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR), // We are prioritizing fast tracing over build time.
                                 m_Device(m_CommandList.GetDevice()), m_RaytracingProperties(raytracingProperties)
    {
    }

    VulkanAccelerationStructure::VulkanAccelerationStructure(VulkanAccelerationStructure&& otherAS) noexcept
                               : m_CommandList(otherAS.m_CommandList), m_BuildFlags(otherAS.m_BuildFlags), m_BuildGeometryInfo(otherAS.m_BuildGeometryInfo), 
                                 m_BuildSizesInfo(otherAS.m_BuildSizesInfo), m_Device(otherAS.m_Device), m_RaytracingProperties(otherAS.m_RaytracingProperties),
                                 m_AccelerationStructure(otherAS.m_AccelerationStructure)
          
    {
        otherAS.m_AccelerationStructure = nullptr;
    }

    VulkanAccelerationStructure::~VulkanAccelerationStructure()
    {
        if (m_AccelerationStructure != nullptr)
        {
            m_CommandList.vkDestroyAccelerationStructureKHR(m_Device.GetHandle(), m_AccelerationStructure, nullptr);
            m_AccelerationStructure = nullptr;
        }
    }
    
    inline uint64_t RoundUp(uint64_t size, uint64_t granularity)
    {
        const uint64_t divUp = (size + granularity - 1) / granularity;
        return divUp * granularity;
    }

    VkAccelerationStructureBuildSizesInfoKHR VulkanAccelerationStructure::GetBuildSizes(const uint32_t* pMaxPrimitiveCounts) const
    {
        // Query both the size of the finished acceleration structure and the amount of scratch memory needed.
        VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

        // Handle, Buld On Device instad of Host, Build Geometry Info, Array of number of primitives per Geometry, Size Storage.
        m_CommandList.vkGetAccelerationStructureBuildSizesKHR(m_Device.GetHandle(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, 
                                                              &m_BuildGeometryInfo, pMaxPrimitiveCounts, &sizeInfo);

        // AccelerationStructure offset needs to be 256 bytes aligned according to Vulkan specifications. https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkAccelerationStructureCreateInfoKHR.html
        const uint64_t accelerationStructureAlignment = 256;
        const uint64_t scratchMemoryAlignment = m_RaytracingProperties.GetMinAccelerationStructureScratchOffsetAlignment();

        sizeInfo.accelerationStructureSize = RoundUp(sizeInfo.accelerationStructureSize, accelerationStructureAlignment);
        sizeInfo.buildScratchSize = RoundUp(sizeInfo.buildScratchSize, scratchMemoryAlignment);

        return sizeInfo;
    }

    void VulkanAccelerationStructure::CreateAccelerationStructure(VulkanBuffer& resultBuffer, VkDeviceSize resultOffset)
    {
        VkAccelerationStructureCreateInfoKHR creationInfo = {};
        creationInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        creationInfo.pNext = nullptr;
        creationInfo.type = m_BuildGeometryInfo.type;
        creationInfo.size = GetBuildSizes().accelerationStructureSize;
        creationInfo.buffer = resultBuffer.GetHandle(); // The buffer on which the acceleration structure will be stored.
        creationInfo.offset = resultOffset; // Offset in bytes from the base address of the buffer at which the AS will be stored. Must be at an offset of 256 per Vulkan Specifications.

        CheckResult(m_CommandList.vkCreateAccelerationStructureKHR(m_Device.GetHandle(), &creationInfo, nullptr, &m_AccelerationStructure), "Acceleration Structure Creation");
    }

    void VulkanAccelerationStructure::MemoryBarrier(VkCommandBuffer commandBuffer)
    {
        // Wait for the builder to complete by setting a barrier on the resulting buffer. This is important as the construction of the top level hierarchy may be called right afterwards, before executing the command list.
        VkMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        memoryBarrier.pNext = nullptr;
        memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
        memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, 0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
    }
}