#pragma once
#include "VulkanAccelerationStructure.h"
#include "VulkanBottomLevelGeometry.h"

namespace Resources
{
    class Procedural;
    class Scene;
}

namespace Vulkan::Raytracing
{
    class VulkanRaytracingProperties;
    class VulkanRaytracingCommandList;

    class VulkanBottomLevelAS final : public VulkanAccelerationStructure
    {
    public:
        VulkanBottomLevelAS(const VulkanRaytracingCommandList& commandList, const VulkanRaytracingProperties& raytracingProperties, const VulkanBottomLevelGeometry& geometry);
        VulkanBottomLevelAS(VulkanBottomLevelAS&& otherAS) noexcept;
        ~VulkanBottomLevelAS();

        void Generate(VkCommandBuffer commandBuffer, VulkanBuffer& scratchBuffer, VkDeviceSize scratchBufferOffset, VulkanBuffer& resultBuffer, VkDeviceSize resultBufferOffset);

    private:
        VulkanBottomLevelGeometry m_Geometries;
    };
}