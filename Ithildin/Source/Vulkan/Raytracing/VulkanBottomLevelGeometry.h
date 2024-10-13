#pragma once
#include "Core/Core.h"
#include <vector>

namespace Resources
{
    class Procedural;
    class Scene;
}

namespace Vulkan::Raytracing
{
    class VulkanBottomLevelGeometry final
    {
    public:
        size_t GetSize() const { return m_Geometries.size(); }

        const std::vector<VkAccelerationStructureGeometryKHR>& GetGeometries() const { return m_Geometries; }
        const std::vector<VkAccelerationStructureBuildRangeInfoKHR>& GetBuildOffsetInfo() const { return m_BuildOffsetInfo; }

        void AddGeometry_Triangles(const Resources::Scene& scene, uint32_t vertexOffset, uint32_t vertexCount, uint32_t indexOffset, uint32_t indexCount, bool isOpaque);
        void AddGeometry_AABB(const Resources::Scene& scene, uint32_t aabbOffset, uint32_t aabbCount, bool isOpaque);

    private:
        // The geometry to build. Contains geometrical information on vertices, indices amongst others.
        std::vector<VkAccelerationStructureGeometryKHR> m_Geometries;
        // The number of elements to build and offsets.
        std::vector<VkAccelerationStructureBuildRangeInfoKHR> m_BuildOffsetInfo;
    };
};