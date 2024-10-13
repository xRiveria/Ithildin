#include "VulkanBottomLevelAS.h"
#include "VulkanRaytracingCommandList.h"
#include "Resources/Scene.h"
#include "Resources/Vertex.h"
#include "Vulkan/VulkanBuffer.h"

namespace Vulkan::Raytracing
{
    VulkanBottomLevelAS::VulkanBottomLevelAS(const VulkanRaytracingCommandList& commandList, const VulkanRaytracingProperties& raytracingProperties, const VulkanBottomLevelGeometry& geometry)
                        : VulkanAccelerationStructure(commandList, raytracingProperties), m_Geometries(geometry)
    {
        m_BuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        m_BuildGeometryInfo.flags = m_BuildFlags;
        m_BuildGeometryInfo.geometryCount = static_cast<uint32_t>(m_Geometries.GetGeometries().size());
        m_BuildGeometryInfo.pGeometries = m_Geometries.GetGeometries().data();
        m_BuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR; // Building
        m_BuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        m_BuildGeometryInfo.srcAccelerationStructure = nullptr; // For updating (refit).
        
        std::vector<uint32_t> maxPrimitiveCount(m_Geometries.GetBuildOffsetInfo().size());

        for (size_t i = 0; i != maxPrimitiveCount.size(); ++i)
        {
            maxPrimitiveCount[i] = m_Geometries.GetBuildOffsetInfo()[i].primitiveCount;
        }

        m_BuildSizesInfo = GetBuildSizes(maxPrimitiveCount.data());
    }

    VulkanBottomLevelAS::VulkanBottomLevelAS(VulkanBottomLevelAS&& otherAS) noexcept  
                                           : VulkanAccelerationStructure(std::move(otherAS)), m_Geometries(std::move(otherAS.m_Geometries))
    {

    }

    VulkanBottomLevelAS::~VulkanBottomLevelAS()
    {

    }

    void VulkanBottomLevelAS::Generate(VkCommandBuffer commandBuffer, VulkanBuffer& scratchBuffer, VkDeviceSize scratchBufferOffset, VulkanBuffer& resultBuffer, VkDeviceSize resultBufferOffset)
    {
        // Create the acceleration structure.
        CreateAccelerationStructure(resultBuffer, resultBufferOffset);

        // Build the actual bottom-level acceleration structure.
        const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = m_Geometries.GetBuildOffsetInfo().data();

        m_BuildGeometryInfo.dstAccelerationStructure = GetHandle();
        m_BuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.GetDeviceAddress() + scratchBufferOffset;

        m_CommandList.vkCmdBuildAccelerationStructuresKHR(commandBuffer, 1, &m_BuildGeometryInfo, &pBuildOffsetInfo);
    }
}