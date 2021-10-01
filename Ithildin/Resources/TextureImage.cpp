#include "TextureImage.h"
#include "Core/Core.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanImage.h"
#include "Vulkan/VulkanImageView.h"
#include "Vulkan/VulkanCommandPool.h"
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanSampler.h"
#include "Texture.h"

namespace Resources
{
    TextureImage::TextureImage(Raytracing::VulkanCommandPool& commandPool, const Texture& texture)
    {
        // Create a host staging buffer and copy the image into it.
        const VkDeviceSize imageSize = texture.GetWidth() * texture.GetHeight() * 4; // Width Pixels * Height Pixels * Components Per Pixel
        const Raytracing::VulkanDevice& device = commandPool.GetDevice();

        std::unique_ptr<Raytracing::VulkanBuffer> stagingBuffer = std::make_unique<Raytracing::VulkanBuffer>(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        Raytracing::VulkanDeviceMemory stagingBufferMemory = stagingBuffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        const auto pointerToGPUBuffer = stagingBufferMemory.Map(0, imageSize);
        std::memcpy(pointerToGPUBuffer, texture.GetPixels(), imageSize); // Copy pixels into our memory.
        stagingBufferMemory.Unmap();

        // Create the device side image, memory, view and sampler.
        m_Image.reset(new Raytracing::VulkanImage(device, VkExtent2D{ static_cast<uint32_t>(texture.GetWidth()), static_cast<uint32_t>(texture.GetHeight()) }, VK_FORMAT_R8G8B8A8_UNORM));
        m_ImageMemory.reset(new Raytracing::VulkanDeviceMemory(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
        m_ImageView.reset(new Raytracing::VulkanImageView(device, m_Image->GetHandle(), m_Image->GetFormat(), VK_IMAGE_ASPECT_COLOR_BIT));
        m_ImageSampler.reset(new Raytracing::VulkanSampler(device, Raytracing::SamplerConfiguration()));

        // Transfer the data to device side.
        m_Image->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        m_Image->CopyFrom(commandPool, *stagingBuffer);
        m_Image->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Delete the buffer before the memory frees at the end of this scope.
        stagingBuffer.reset();
    }

    TextureImage::~TextureImage()
    {
        m_ImageSampler.reset();
        m_ImageView.reset();
        m_Image.reset();
        m_ImageMemory.reset();
    }
}