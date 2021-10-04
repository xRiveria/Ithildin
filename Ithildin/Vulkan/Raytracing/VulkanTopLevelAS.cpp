#include "VulkanTopLevelAS.h"
#include "VulkanBottomLevelAS.h"
#include "VulkanRaytracingCommandList.h"
#include "../VulkanBuffer.h"
#include "../VulkanDevice.h"
#include <string>

namespace Vulkan::Raytracing
{
    VulkanTopLevelAS::VulkanTopLevelAS(const VulkanRaytracingCommandList& commandList, const VulkanRaytracingProperties& raytracingProperties, VkDeviceAddress instanceAddress, uint32_t instancesCount)
                                     : VulkanAccelerationStructure(commandList, raytracingProperties), m_InstancesCount(instancesCount)
    {
        // Create VkAccelerationStructureGeometryInstancesDataKHGR. This wraps a device pointer to the above uploaded instances.
        m_VulkanASInstancesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
        m_VulkanASInstancesInfo.arrayOfPointers = VK_FALSE;
        m_VulkanASInstancesInfo.data.deviceAddress = instanceAddress;

        // Put the above into a VkAccelerationStructureGeometryKHR, similar to what we did with our triangle/AABB data in the BLAS.
        m_TopASGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        m_TopASGeometryInfo.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
        m_TopASGeometryInfo.geometry.instances = m_VulkanASInstancesInfo;

        m_BuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        m_BuildGeometryInfo.flags = m_BuildFlags;
        m_BuildGeometryInfo.geometryCount = 1;
        m_BuildGeometryInfo.pGeometries = &m_TopASGeometryInfo;
        m_BuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        m_BuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        m_BuildGeometryInfo.srcAccelerationStructure = nullptr;

        m_BuildSizesInfo = GetBuildSizes(&instancesCount);
    }

    VulkanTopLevelAS::VulkanTopLevelAS(VulkanTopLevelAS&& otherAS) noexcept : VulkanAccelerationStructure(std::move(otherAS)), m_InstancesCount(otherAS.m_InstancesCount)
    {

    }

    VulkanTopLevelAS::~VulkanTopLevelAS()
    {

    }

    void VulkanTopLevelAS::Generate(VkCommandBuffer commandBuffer, VulkanBuffer& scratchBuffer, VkDeviceSize scratchBufferOffset, VulkanBuffer& resultBuffer, VkDeviceSize resultBufferOffset)
    {
        // Create the acceleration structure.
        CreateAccelerationStructure(resultBuffer, resultBufferOffset);

        // Build the actual top-level acceleration structure.
        VkAccelerationStructureBuildRangeInfoKHR buildOffsetInfo = {};
        buildOffsetInfo.primitiveCount = m_InstancesCount;

        const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;

        m_BuildGeometryInfo.dstAccelerationStructure = GetHandle();
        m_BuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.GetDeviceAddress() + scratchBufferOffset;

        m_CommandList.vkCmdBuildAccelerationStructuresKHR(commandBuffer, 1, &m_BuildGeometryInfo, &pBuildOffsetInfo);
    }

    VkAccelerationStructureInstanceKHR VulkanTopLevelAS::CreateASInstance(const VulkanBottomLevelAS& bottomLevelAS, const glm::mat4& transform, uint32_t instanceID, uint32_t hitGroupID)
    {
        const VulkanDevice& device = bottomLevelAS.GetDevice();
        const VulkanRaytracingCommandList& commandList = bottomLevelAS.GetCommandList();

        // Obtain address of our BLAS.
        VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {};
        addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        addressInfo.accelerationStructure = bottomLevelAS.GetHandle();

        const VkDeviceAddress deviceAddress = commandList.vkGetAccelerationStructureDeviceAddressKHR(device.GetHandle(), &addressInfo);

        VkAccelerationStructureInstanceKHR instanceInfo = {};
        instanceInfo.instanceCustomIndex = instanceID; // Allows for shader access.
        // A ray can intersect an instance only if the bitwise AND of this mask and the ray's mask is non-zero.
        instanceInfo.mask = 0xFF; // The visibility mask is always set to 0xFF, but if some instances would need to be ignored in some cases, this flag should be passed by the application.
        instanceInfo.instanceShaderBindingTableRecordOffset = hitGroupID; // Sets the hit group ID. This will be used to find the shadedr code to excute when hitting the geometry.
        instanceInfo.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR; // Disable culling.
        instanceInfo.accelerationStructureReference = deviceAddress;

        // The instance.transform value only contains 12 values, corresponding to a 3x4 matrix, hence saving the last row that is anyway always (0, 0, 0, 1). Since the matrix is row-major, we simply copy the first 12 values of the original 4x4 matrix.
        std::memcpy(&instanceInfo.transform, &transform, sizeof(instanceInfo.transform));

        return instanceInfo;
    }
}