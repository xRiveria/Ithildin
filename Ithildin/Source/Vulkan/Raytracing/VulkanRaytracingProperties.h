#pragma once
#include "../VulkanDevice.h"

namespace Vulkan
{
    class VulkanDevice;

    namespace Raytracing
    {
        class VulkanRaytracingProperties final
        {
        public:
            explicit VulkanRaytracingProperties(const VulkanDevice& device);

            const VulkanDevice& GetDevice() const { return m_Device; }

            uint64_t GetMaxDescriptorSetAccelerationStructures() const { return m_AccelerationStructureProperties.maxDescriptorSetAccelerationStructures; } // Max AS descriptors that can be included in descriptor bindings in a pipeline layout.
            uint64_t GetMaxGeometryCount() const { return m_AccelerationStructureProperties.maxGeometryCount; }   // Max Geometries in BLAS
            uint64_t GetMaxInstanceCount() const { return m_AccelerationStructureProperties.maxInstanceCount; }   // Max Instances in TLAS
            uint64_t GetMaxPrimitiveCount() const { return m_AccelerationStructureProperties.maxPrimitiveCount; } // Max number of triangles or AABBs in all geometries in the BLAS.
            uint32_t GetMaxRecursionDepth() const { return m_RaytracingPipelineProperties.maxRayRecursionDepth; } // Max number of levels of ray recursion allowed in a trace command.
            uint32_t GetMaxShaderGroupStride() const { return m_RaytracingPipelineProperties.maxShaderGroupStride; } // Max stride in bytes allowed between shader groups in the SBT.
            uint32_t GetMinAccelerationStructureScratchOffsetAlignment() const { return m_AccelerationStructureProperties.minAccelerationStructureScratchOffsetAlignment; } // Minimum required alignment in bytes for scratch data passed in to an acceleration structure build command.
            uint32_t GetShaderGroupBaseAlignment() const { return m_RaytracingPipelineProperties.shaderGroupBaseAlignment; } // Required alignment in bytes for the base of the SBT.
            uint32_t GetShaderGroupHandleCaptureReplaySize() const { return m_RaytracingPipelineProperties.shaderGroupHandleCaptureReplaySize; } // Number of bytes for the information required to do capture and reply for shader group handles.
            uint32_t GetShaderGroupHandleSize() const { return m_RaytracingPipelineProperties.shaderGroupHandleSize; } // Size in bytes of the shader header.

        private:
            const VulkanDevice& m_Device;
            VkPhysicalDeviceAccelerationStructurePropertiesKHR m_AccelerationStructureProperties = {};
            VkPhysicalDeviceRayTracingPipelinePropertiesKHR m_RaytracingPipelineProperties = {};
        };
    }
}
