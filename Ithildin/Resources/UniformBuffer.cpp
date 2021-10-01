#include "UniformBuffer.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanDeviceMemory.h"
#include "vulkan/VulkanBuffer.h"

namespace Resources
{
    UniformBuffer::UniformBuffer(const Raytracing::VulkanDevice& device)
    {
        const size_t bufferSize = sizeof(UniformBufferObject);

        m_Buffer.reset(new Raytracing::VulkanBuffer(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
        m_Memory.reset(new Raytracing::VulkanDeviceMemory(m_Buffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)));
    }

    UniformBuffer::UniformBuffer(UniformBuffer&& otherBuffer) noexcept : m_Buffer(otherBuffer.m_Buffer.release()), m_Memory(otherBuffer.m_Memory.release())
    {

    }

    UniformBuffer::~UniformBuffer()
    {
        m_Buffer.reset();
        m_Memory.reset(); // Release memory after bound buffer has been destroyed.
    }

    void UniformBuffer::SetValue(const UniformBufferObject& uniformBufferObject)
    {
        const auto dataPointer = m_Memory->Map(0, sizeof(UniformBufferObject));
        std::memcpy(dataPointer, &uniformBufferObject, sizeof(uniformBufferObject));
        m_Memory->Unmap();
    }
}