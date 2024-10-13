#pragma once
#include "Core/Core.h"
#include <memory>
#include <vector>
#include "../VulkanBuffer.h"

namespace Vulkan
{
    class VulkanDevice;
    class VulkanDeviceMemory;
}

namespace Vulkan::Raytracing
{
    class VulkanRaytracingCommandList;
    class VulkanRaytracingPipeline;
    class VulkanRaytracingProperties;

    class VulkanShaderBindingTable final
    {
    public:
        struct Entry
        {
            uint32_t m_GroupIndex;
            std::vector<unsigned char> m_InlineData;
        };

        VulkanShaderBindingTable(const VulkanRaytracingCommandList& commandList, const VulkanRaytracingPipeline& raytracingPipeline,
                                 const VulkanRaytracingProperties& raytracingProperties, const std::vector<Entry>& rayGenerationPrograms,
                                 const std::vector<Entry>& missPrograms, const std::vector<Entry>& hitGroups);
        ~VulkanShaderBindingTable();

        const VulkanBuffer& GetBuffer() const { return *m_Buffer; }

        VkDeviceAddress GetRayGenerationShaderDeviceAddress() const { return m_Buffer->GetDeviceAddress() + m_RayGenerationShaderDeviceOffset; }
        VkDeviceAddress GetMissShaderDeviceAddress() const { return m_Buffer->GetDeviceAddress() + m_MissShaderDeviceOffset; }
        VkDeviceAddress GetHitGroupDeviceAddress() const { return m_Buffer->GetDeviceAddress() + m_HitGroupDeviceOffset; }

        size_t GetRayGenerationShaderEntrySize() const { return m_RayGenerationShaderEntrySize; }
        size_t GetMissShaderEntrySize() const { return m_MissShaderEntrySize; }
        size_t GetHitGroupEntrySize() const { return m_HitGroupEntrySize; }

        size_t GetRayGenerationShaderSize() const { return m_RayGenerationShaderSize; }
        size_t GetMissShaderSize() const { return m_MissShaderSize; }
        size_t GetHitGroupSize() const { return m_HitGroupSize; }
        
    private:
        const size_t m_RayGenerationShaderEntrySize;
        const size_t m_MissShaderEntrySize;
        const size_t m_HitGroupEntrySize;

        const size_t m_RayGenerationShaderDeviceOffset;
        const size_t m_MissShaderDeviceOffset;
        const size_t m_HitGroupDeviceOffset;

        const size_t m_RayGenerationShaderSize;
        const size_t m_MissShaderSize;
        const size_t m_HitGroupSize;

        std::unique_ptr<VulkanBuffer> m_Buffer;
        std::unique_ptr<VulkanDeviceMemory> m_BufferMemory;
    };
}