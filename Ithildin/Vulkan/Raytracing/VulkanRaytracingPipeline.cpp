#include "VulkanRaytracingPipeline.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanSwapChain.h"
#include "Vulkan/VulkanDescriptorBinding.h"
#include "Vulkan/VulkanDescriptorSetManager.h"
#include "vulkan/VulkanDescriptorSets.h"
#include "Vulkan/VulkanImageView.h"
#include "Vulkan/VulkanShaderModule.h"

namespace Vulkan::Raytracing
{
    Raytracing::VulkanRaytracingPipeline::VulkanRaytracingPipeline(const VulkanRaytracingCommandList& commandList, 
                                                                   const VulkanSwapChain& swapChain, 
                                                                   const VulkanTopLevelAS& accelerationStructure, 
                                                                   const VulkanImageView& accumulationImageView, 
                                                                   const VulkanImageView& outputImageView, 
                                                                   const std::vector<Resources::UniformBuffer>& uniformBuffers, 
                                                                   const Resources::Scene& scene) : m_SwapChain(swapChain)
    {
        // Create descriptor pool/sets.
        const VulkanDevice& device = swapChain.GetDevice();
        const std::vector<VulkanDescriptorBinding> descriptorBindings =
        {
            // Top Level Acceleration Structure

            // Image Accumulation & Output

            // Camera Information & Others

            // Vertex Buffer, Index Buffer, Material buffer, Offset Buffer

            // Textures and Image samplers

            // Procedural Buffer
        };

    }

    Raytracing::VulkanRaytracingPipeline::~VulkanRaytracingPipeline()
    {

    }

    VkDescriptorSet Raytracing::VulkanRaytracingPipeline::GetDescriptorSet(uint32_t index) const
    {
        return VkDescriptorSet();
    }
}