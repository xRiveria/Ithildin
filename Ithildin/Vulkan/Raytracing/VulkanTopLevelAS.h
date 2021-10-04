#pragma once
#include "VulkanAccelerationStructure.h"
#include "Math/Math.h"
#include <vector>

namespace Vulkan::Raytracing
{
    class VulkanBottomLevelAS;
    class VulkanRaytracingCommandList;
    class VulkanRaytracingProperties;

    class VulkanTopLevelAS final : public VulkanAccelerationStructure
    {
    public:
        VulkanTopLevelAS(const VulkanRaytracingCommandList& commandList, const VulkanRaytracingProperties& raytracingProperties, VkDeviceAddress instanceAddress, uint32_t instancesCount);
        VulkanTopLevelAS(VulkanTopLevelAS&& otherAS) noexcept;
        virtual ~VulkanTopLevelAS();

        void Generate(VkCommandBuffer commandBuffer, VulkanBuffer& scratchBuffer, VkDeviceSize scratchBufferOffset, VulkanBuffer& resultBuffer, VkDeviceSize resultBufferOffset);
        static VkAccelerationStructureInstanceKHR CreateASInstance(const VulkanBottomLevelAS& bottomLevelAS, const glm::mat4& transform, uint32_t instanceID, uint32_t hitGroupID);

    private:
        uint32_t m_InstancesCount;
        VkAccelerationStructureGeometryInstancesDataKHR m_VulkanASInstancesInfo = {};
        VkAccelerationStructureGeometryKHR m_TopASGeometryInfo = {};        
    };
}