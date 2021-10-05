#pragma once
#include "Core/Core.h"

namespace Vulkan
{
    class VulkanDevice;
    class VulkanBuffer;
    class VulkanDeviceMemory;

    namespace Raytracing
    {
        class VulkanRaytracingCommandList;
        class VulkanRaytracingProperties;

        class VulkanAccelerationStructure
        {
        public:
            VulkanAccelerationStructure(VulkanAccelerationStructure&& otherAS) noexcept;
            virtual ~VulkanAccelerationStructure();

            const VulkanDevice& GetDevice() const { return m_Device; }
            const VulkanRaytracingCommandList& GetCommandList() const { return m_CommandList; }
            const VkAccelerationStructureBuildSizesInfoKHR GetBuildSizes() const { return m_BuildSizesInfo; }

            static void MemoryBarrier(VkCommandBuffer commandBuffer);

        protected:
            explicit VulkanAccelerationStructure(const VulkanRaytracingCommandList& commandList, const VulkanRaytracingProperties& raytracingProperties);

            VkAccelerationStructureBuildSizesInfoKHR GetBuildSizes(const uint32_t* pMaxPrimitiveCounts) const;
            void CreateAccelerationStructure(VulkanBuffer& resultBuffer, VkDeviceSize resultOffset);

        protected:
            const VulkanRaytracingCommandList& m_CommandList;
            const VkBuildAccelerationStructureFlagsKHR m_BuildFlags;
            VkAccelerationStructureBuildGeometryInfoKHR m_BuildGeometryInfo = {};
            VkAccelerationStructureBuildSizesInfoKHR m_BuildSizesInfo = {};

        private:
            const VulkanDevice& m_Device;
            const VulkanRaytracingProperties& m_RaytracingProperties;

            VULKAN_HANDLE(VkAccelerationStructureKHR, m_AccelerationStructure)
        };

    }
}