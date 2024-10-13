#include "VulkanShaderBindingTable.h"
#include "VulkanRaytracingCommandList.h"
#include "VulkanRaytracingPipeline.h"
#include "VulkanRaytracingProperties.h"
#include "../VulkanDevice.h"
#include "../VulkanDeviceMemory.h"

namespace Vulkan::Raytracing
{
    namespace SBTUtilities
    {
        // Compute the actual size needed per SBT entry by rounding up the alignment needed.
        size_t RoundUp(size_t size, size_t powerOf2Alignment)
        {
            return (size + powerOf2Alignment - 1) & ~(powerOf2Alignment - 1);
        }

        size_t GetEntrySize(const VulkanRaytracingProperties& raytracingProperties, const std::vector<VulkanShaderBindingTable::Entry>& entries)
        {
            // Find the maximum number of parameters used by a single entry.
            size_t maxArguments = 0;

            for (const auto& entry : entries)
            {
                maxArguments = std::max(maxArguments, entry.m_InlineData.size());
            }

            // A SBT entry is made up of a program ID and a set of 4-byte parameters (see shaderRecordEXT).
            // Its size is ShaderGroupHandleSize (plus our parameters) and must be aligned to ShaderGroupBaseAlignment.
            return RoundUp(raytracingProperties.GetShaderGroupHandleSize() + maxArguments, raytracingProperties.GetShaderGroupBaseAlignment());
        }

        size_t CopyShaderData(uint8_t* const destination, const VulkanRaytracingProperties& raytracingProperties, const std::vector<VulkanShaderBindingTable::Entry>& entries,
                              const size_t entrySize, const uint8_t* const shaderHandleStorage)
        {
            const uint32_t handleSize = raytracingProperties.GetShaderGroupHandleSize();
            uint8_t* pDestination = destination;

            for (const auto& entry : entries)
            {
                // Copy the shader identifier that was previously obtained with vkGetRayTracingShaderGroupHandlesKHR.
                std::memcpy(pDestination, shaderHandleStorage + entry.m_GroupIndex * handleSize, handleSize);
                std::memcpy(pDestination + handleSize, entry.m_InlineData.data(), entry.m_InlineData.size());

                pDestination += entrySize;
            }

            return entries.size() * entrySize;
        }
    }

    Raytracing::VulkanShaderBindingTable::VulkanShaderBindingTable(const VulkanRaytracingCommandList& commandList, const VulkanRaytracingPipeline& raytracingPipeline, const VulkanRaytracingProperties& raytracingProperties, const std::vector<Entry>& rayGenerationPrograms, const std::vector<Entry>& missPrograms, const std::vector<Entry>& hitGroups)
        : m_RayGenerationShaderEntrySize(SBTUtilities::GetEntrySize(raytracingProperties, rayGenerationPrograms)),
          m_MissShaderEntrySize(SBTUtilities::GetEntrySize(raytracingProperties, missPrograms)),
          m_HitGroupEntrySize(SBTUtilities::GetEntrySize(raytracingProperties, hitGroups)),
         
          m_RayGenerationShaderDeviceOffset(0),
          m_MissShaderDeviceOffset(rayGenerationPrograms.size() * m_RayGenerationShaderEntrySize),
          m_HitGroupDeviceOffset(m_MissShaderDeviceOffset + missPrograms.size() * m_MissShaderEntrySize),
          
          m_RayGenerationShaderSize(rayGenerationPrograms.size() * m_RayGenerationShaderEntrySize),
          m_MissShaderSize(missPrograms.size() * m_MissShaderEntrySize),
          m_HitGroupSize(hitGroups.size() * m_HitGroupEntrySize)
    {
        // Compute the size of the table.
        const size_t shaderBindingTableSize = m_RayGenerationShaderSize + m_MissShaderSize + m_HitGroupSize;

        // Allocate buffer and memory.
        const VulkanDevice& device = raytracingProperties.GetDevice();

        m_Buffer.reset(new VulkanBuffer(device, shaderBindingTableSize, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR));
        m_BufferMemory.reset(new VulkanDeviceMemory(m_Buffer->AllocateMemory(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)));

        // Generate the table.
        const uint32_t handleSize = raytracingProperties.GetShaderGroupHandleSize();
        const size_t groupCount = rayGenerationPrograms.size() + missPrograms.size() + hitGroups.size();
        std::vector<uint8_t> shaderHandleStorage(handleSize * groupCount);

        CheckResult(commandList.vkGetRayTracingShaderGroupHandlesKHR(
            device.GetHandle(), raytracingPipeline.GetHandle(), 0, static_cast<uint32_t>(groupCount), shaderHandleStorage.size(), shaderHandleStorage.data()),
            "Getting Raytracing Shader Group Handles");

        // Copy the shader identifiers, followed by their resource pointers or root contants.
        // First the ray generation shaders, then the miss shaders and finally the set of hit groups.
        uint8_t* pointerToDeviceData = static_cast<uint8_t*>(m_BufferMemory->Map(0, shaderBindingTableSize));

        pointerToDeviceData += SBTUtilities::CopyShaderData(pointerToDeviceData, raytracingProperties, rayGenerationPrograms, m_RayGenerationShaderEntrySize, shaderHandleStorage.data());
        pointerToDeviceData += SBTUtilities::CopyShaderData(pointerToDeviceData, raytracingProperties, missPrograms, m_MissShaderEntrySize, shaderHandleStorage.data());
        SBTUtilities::CopyShaderData(pointerToDeviceData, raytracingProperties, hitGroups, m_HitGroupEntrySize, shaderHandleStorage.data());

        m_BufferMemory->Unmap();
    }

    Raytracing::VulkanShaderBindingTable::~VulkanShaderBindingTable()
    {
        m_Buffer.reset();
        m_BufferMemory.reset();
    }

}