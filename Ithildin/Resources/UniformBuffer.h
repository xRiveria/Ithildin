#pragma once
#include "Math/Math.h"
#include <memory>

namespace Raytracing
{
    class VulkanBuffer;
    class VulkanDevice;
    class VulkanDeviceMemory;
}

namespace Resources
{
    class UniformBufferObject
    {
        glm::mat4 m_ModelView;
        glm::mat4 m_Projection;
        glm::mat4 m_ModelViewInverse;
        glm::mat4 m_ProjectionInverse;
        float m_Aperture;
        float m_FocusDistance;
        float m_HeatmapScale;
        uint32_t m_TotalSamplesCount;
        uint32_t m_SampleCount;
        uint32_t m_BounceCount;
        uint32_t m_RandomSeed;
        uint32_t m_HasSky; // Bool
        uint32_t m_ShowHeatMap; // Bool
    };

    class UniformBuffer
    {
    public:
        explicit UniformBuffer(const Raytracing::VulkanDevice& device);
        UniformBuffer(UniformBuffer&& otherBuffer) noexcept;
        ~UniformBuffer();

        const Raytracing::VulkanBuffer& GetBuffer() const { return *m_Buffer; }

        void SetValue(const UniformBufferObject& uniformBufferObject);

    private:
        std::unique_ptr<Raytracing::VulkanBuffer> m_Buffer;
        std::unique_ptr<Raytracing::VulkanDeviceMemory> m_Memory;
    };
}