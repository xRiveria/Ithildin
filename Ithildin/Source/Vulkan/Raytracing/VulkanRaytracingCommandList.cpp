#include "VulkanRaytracingCommandList.h"
#include "../VulkanDevice.h"
#include <stdexcept>

namespace Vulkan::Raytracing
{
    namespace QueryTracingCommand
    {
        template<typename Function>
        Function GetProcedure(const VulkanDevice& device, const char* const queryFunctionName)
        {
            const auto queriedFunction = reinterpret_cast<Function>(vkGetDeviceProcAddr(device.GetHandle(), queryFunctionName));
            if (queriedFunction)
            {
                std::runtime_error(std::string("Failed to get address of: ") + queryFunctionName);
            }

            return queriedFunction;
        }
    }

    Raytracing::VulkanRaytracingCommandList::VulkanRaytracingCommandList(const VulkanDevice& device) :
        vkCreateAccelerationStructureKHR(QueryTracingCommand::GetProcedure<PFN_vkCreateAccelerationStructureKHR>(device, "vkCreateAccelerationStructureKHR")),
        vkDestroyAccelerationStructureKHR(QueryTracingCommand::GetProcedure<PFN_vkDestroyAccelerationStructureKHR>(device, "vkDestroyAccelerationStructureKHR")),
        vkGetAccelerationStructureBuildSizesKHR(QueryTracingCommand::GetProcedure<PFN_vkGetAccelerationStructureBuildSizesKHR>(device, "vkGetAccelerationStructureBuildSizesKHR")),
        vkCmdBuildAccelerationStructuresKHR(QueryTracingCommand::GetProcedure<PFN_vkCmdBuildAccelerationStructuresKHR>(device, "vkCmdBuildAccelerationStructuresKHR")),
        vkCmdCopyAccelerationStructureKHR(QueryTracingCommand::GetProcedure<PFN_vkCmdCopyAccelerationStructureKHR>(device, "vkCmdCopyAccelerationStructureKHR")),
        vkCmdTraceRaysKHR(QueryTracingCommand::GetProcedure<PFN_vkCmdTraceRaysKHR>(device, "vkCmdTraceRaysKHR")),
        vkCreateRayTracingPipelinesKHR(QueryTracingCommand::GetProcedure<PFN_vkCreateRayTracingPipelinesKHR>(device, "vkCreateRayTracingPipelinesKHR")),
        vkGetRayTracingShaderGroupHandlesKHR(QueryTracingCommand::GetProcedure<PFN_vkGetRayTracingShaderGroupHandlesKHR>(device, "vkGetRayTracingShaderGroupHandlesKHR")),
        vkGetAccelerationStructureDeviceAddressKHR(QueryTracingCommand::GetProcedure<PFN_vkGetAccelerationStructureDeviceAddressKHR>(device, "vkGetAccelerationStructureDeviceAddressKHR")),
        vkCmdWriteAccelerationStructuresPropertiesKHR(QueryTracingCommand::GetProcedure<PFN_vkCmdWriteAccelerationStructuresPropertiesKHR>(device, "vkCmdWriteAccelerationStructuresPropertiesKHR")),
        m_Device(device)
    {

    }

    Raytracing::VulkanRaytracingCommandList::~VulkanRaytracingCommandList()
    {
    }
}
