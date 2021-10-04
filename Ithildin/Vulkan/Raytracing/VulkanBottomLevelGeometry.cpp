#include "VulkanBottomLevelGeometry.h"
#include "VulkanRaytracingCommandList.h"
#include "Resources/Scene.h"
#include "Resources/Vertex.h"
#include "vulkan/VulkanBuffer.h"

namespace Vulkan::Raytracing
{
    void VulkanBottomLevelGeometry::AddGeometry_Triangles(const Resources::Scene& scene, uint32_t vertexOffset, uint32_t vertexCount, uint32_t indexOffset, uint32_t indexCount, bool isOpaque)
    {
        // Represents our geometry data.
        VkAccelerationStructureGeometryKHR geometryInfo = {};
        geometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        geometryInfo.pNext = nullptr;
        geometryInfo.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        geometryInfo.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        geometryInfo.geometry.triangles.pNext = nullptr;
        geometryInfo.geometry.triangles.vertexData.deviceAddress = scene.GetVertexBuffer().GetDeviceAddress();
        geometryInfo.geometry.triangles.vertexStride = sizeof(Resources::Vertex);
        geometryInfo.geometry.triangles.maxVertex = vertexCount;
        geometryInfo.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        geometryInfo.geometry.triangles.indexData.deviceAddress = scene.GetIndexBuffer().GetDeviceAddress();
        geometryInfo.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
        geometryInfo.geometry.triangles.transformData = {}; // Any transformation manipulations before the BLAS is built. Else, use the identity matrix.
        geometryInfo.flags = isOpaque ? VK_GEOMETRY_OPAQUE_BIT_KHR : 0; // If opaque, indicate that the geometry does not invoke Any-Hit shaders even if present in a hit group.
    
        VkAccelerationStructureBuildRangeInfoKHR buildOffsetInfo = {};
        buildOffsetInfo.firstVertex = vertexOffset / sizeof(Resources::Vertex);
        buildOffsetInfo.primitiveOffset = indexOffset;
        buildOffsetInfo.primitiveCount = indexCount / 3; 
        buildOffsetInfo.transformOffset = 0;

        m_Geometries.emplace_back(geometryInfo);
        m_BuildOffsetInfo.emplace_back(buildOffsetInfo);
    }

    void VulkanBottomLevelGeometry::AddGeometry_AABB(const Resources::Scene& scene, uint32_t aabbOffset, uint32_t aabbCount, bool isOpaque)
    {
        VkAccelerationStructureGeometryKHR geometryInfo = {};
        geometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        geometryInfo.pNext = nullptr;
        geometryInfo.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
        geometryInfo.geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
        geometryInfo.geometry.aabbs.pNext = nullptr;
        geometryInfo.geometry.aabbs.data.deviceAddress = scene.GetAABBBuffer().GetDeviceAddress();
        geometryInfo.geometry.aabbs.stride = sizeof(VkAabbPositionsKHR);
        geometryInfo.flags = isOpaque ? VK_GEOMETRY_OPAQUE_BIT_KHR : 0;

        VkAccelerationStructureBuildRangeInfoKHR buildOffsetInfo = {};
        buildOffsetInfo.firstVertex = 0;
        buildOffsetInfo.primitiveOffset = aabbOffset;
        buildOffsetInfo.primitiveCount = aabbCount;
        buildOffsetInfo.transformOffset = 0;

        m_Geometries.emplace_back(geometryInfo);
        m_BuildOffsetInfo.emplace_back(buildOffsetInfo);
    }
}