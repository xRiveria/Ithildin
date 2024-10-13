#include "RaytracingApplication.h"
#include "../VulkanInstance.h"
#include "../VulkanDebugMessenger.h"
#include "../VulkanSurface.h"
#include "../VulkanDevice.h"
#include "../VulkanImageView.h"
#include "../VulkanCommandPool.h"
#include "../VulkanSwapChain.h"
#include "../VulkanBuffer.h"
#include "../VulkanDepthBuffer.h"
#include "../VulkanFence.h"
#include "../VulkanSemaphore.h"
#include "../VulkanPipelineLayout.h"
#include "../VulkanGraphicsPipeline.h"
#include "../VulkanFramebuffer.h"
#include "../VulkanCommandBuffers.h"
#include "../VulkanRenderPass.h"
#include "../SingleTimeCommands.h"
#include "VulkanRaytracingCommandList.h"
#include "VulkanRaytracingProperties.h"
#include "VulkanBottomLevelAS.h"
#include "VulkanTopLevelAS.h"
#include "VulkanBottomLevelGeometry.h"
#include "VulkanRaytracingPipeline.h"
#include "VulkanShaderBindingTable.h"
#include "../VulkanBufferUtilities.h"
#include "../VulkanImageMemoryBarrier.h"
#include "Resources/UniformBuffer.h"
#include "Resources/Scene.h"
#include "Resources/Model.h"
#include "Resources/Texture.h"
#include "Core/Window.h"
#include <string>
#include <chrono>

namespace Vulkan::Raytracing
{
    namespace ASUtilities
    {
        template<typename TAccelerationStructure>
        VkAccelerationStructureBuildSizesInfoKHR GetTotalRequirements(const std::vector<TAccelerationStructure>& accelerationStructures)
        {
            VkAccelerationStructureBuildSizesInfoKHR totalSize = {};

            for (const auto& accelerationStructure : accelerationStructures)
            {
                totalSize.accelerationStructureSize += accelerationStructure.GetBuildSizes().accelerationStructureSize;
                totalSize.buildScratchSize += accelerationStructure.GetBuildSizes().buildScratchSize;
                totalSize.updateScratchSize += accelerationStructure.GetBuildSizes().updateScratchSize;
            }

            return totalSize;
        }
    }

    RaytracingApplication::RaytracingApplication(const WindowSettings& windowSettings, VkPresentModeKHR requestedPresentationMode, bool enabledValidationLayers)
                           : Vulkan::Application(windowSettings, requestedPresentationMode, enabledValidationLayers)
    {
    }

    RaytracingApplication::~RaytracingApplication()
    {
        RaytracingApplication::DeleteSwapChain();
        DeleteAccelerationStructures();

        m_RaytracingProperties.reset();
        m_RaytracingCommandList.reset();
    }

    void RaytracingApplication::SetPhysicalDevice(VkPhysicalDevice physicalDevice, std::vector<const char*>& requiredExtensions,
                                                    VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures)
    {
        // Required extensions.
        requiredExtensions.insert(requiredExtensions.end(),
        {
            VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
            VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
            VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
        });

        VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
        bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
        bufferDeviceAddressFeatures.pNext = nextDeviceFeatures;
        bufferDeviceAddressFeatures.bufferDeviceAddress = true;

        VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures = {};
        indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        indexingFeatures.pNext = &bufferDeviceAddressFeatures;
        indexingFeatures.runtimeDescriptorArray = true;
        indexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;

        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};
        accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        accelerationStructureFeatures.pNext = &indexingFeatures;
        accelerationStructureFeatures.accelerationStructure = true;

        VkPhysicalDeviceRayTracingPipelineFeaturesKHR raytracingFeatures = {};
        raytracingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        raytracingFeatures.pNext = &accelerationStructureFeatures;
        raytracingFeatures.rayTracingPipeline = true;

        Vulkan::Application::SetPhysicalDevice(physicalDevice, requiredExtensions, deviceFeatures, &raytracingFeatures);
    }

    void RaytracingApplication::OnDeviceSet()
    {
        Vulkan::Application::OnDeviceSet();

        m_RaytracingCommandList.reset(new VulkanRaytracingCommandList(GetDevice()));
        m_RaytracingProperties.reset(new VulkanRaytracingProperties(GetDevice()));
    }

    void RaytracingApplication::CreateSwapChain()
    {
        Vulkan::Application::CreateSwapChain();

        CreateOutputImage();

        m_RaytracingPipeline.reset(new VulkanRaytracingPipeline(*m_RaytracingCommandList, GetSwapChain(), m_TopAccelerationStructures[0],
            *m_AccumulationImageView, *m_OutputImageView, GetUniformBuffers(), GetScene()));

        const std::vector<VulkanShaderBindingTable::Entry> rayGenerationPrograms = { { m_RaytracingPipeline->GetRayGenerationShaderIndex(), {}} };
        const std::vector<VulkanShaderBindingTable::Entry> missPrograms = { { m_RaytracingPipeline->GetMissShaderIndex(), {} } };
        const std::vector<VulkanShaderBindingTable::Entry> hitGroups = { { m_RaytracingPipeline->GetTriangleHitGroupIndex(), {} }, { m_RaytracingPipeline->GetProceduralHitGroupIndex(), {} } };

        m_ShaderBindingTable.reset(new VulkanShaderBindingTable(*m_RaytracingCommandList, *m_RaytracingPipeline, *m_RaytracingProperties, rayGenerationPrograms, missPrograms, hitGroups));
    }

    void RaytracingApplication::DeleteSwapChain()
    {
        m_ShaderBindingTable.reset();
        m_RaytracingPipeline.reset();
        m_OutputImageView.reset();
        m_OutputImage.reset();
        m_OutputImageMemory.reset();
        m_AccumulationImageView.reset();
        m_AccumulationImage.reset();
        m_AccumulationImageMemory.reset();

        Vulkan::Application::DeleteSwapChain();
    }

    void RaytracingApplication::Render(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        const VkExtent2D extent = GetSwapChain().GetExtent();

        VkDescriptorSet descriptorSets[] = { m_RaytracingPipeline->GetDescriptorSet(imageIndex) };

        VkImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.layerCount = 1;

        // Acquire destination images for rendering.
        VulkanImageMemoryBarrier::Insert(commandBuffer, m_AccumulationImage->GetHandle(), subresourceRange, 0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
        VulkanImageMemoryBarrier::Insert(commandBuffer, m_OutputImage->GetHandle(), subresourceRange, 0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

        // Bind raytracing pipeline.
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_RaytracingPipeline->GetHandle());
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_RaytracingPipeline->GetPipelineLayout().GetHandle(), 0, 1, descriptorSets, 0, nullptr);

        // Describe the shader binding table.
        VkStridedDeviceAddressRegionKHR rayGenerationShaderBindingTable = {};
        rayGenerationShaderBindingTable.deviceAddress = m_ShaderBindingTable->GetRayGenerationShaderDeviceAddress();
        rayGenerationShaderBindingTable.stride = m_ShaderBindingTable->GetRayGenerationShaderEntrySize();
        rayGenerationShaderBindingTable.size = m_ShaderBindingTable->GetRayGenerationShaderSize();

        VkStridedDeviceAddressRegionKHR missShaderBindingTable = {};
        missShaderBindingTable.deviceAddress = m_ShaderBindingTable->GetMissShaderDeviceAddress();
        missShaderBindingTable.stride = m_ShaderBindingTable->GetMissShaderEntrySize();
        missShaderBindingTable.size = m_ShaderBindingTable->GetMissShaderSize();

        VkStridedDeviceAddressRegionKHR hitShaderBindingTable = {};
        hitShaderBindingTable.deviceAddress = m_ShaderBindingTable->GetHitGroupDeviceAddress();
        hitShaderBindingTable.stride = m_ShaderBindingTable->GetHitGroupEntrySize();
        hitShaderBindingTable.size = m_ShaderBindingTable->GetHitGroupSize();

        VkStridedDeviceAddressRegionKHR callableShaderBindingTable = {};

        // Eexecute Raytracing Shaders
        m_RaytracingCommandList->vkCmdTraceRaysKHR(commandBuffer, &rayGenerationShaderBindingTable, &missShaderBindingTable, &hitShaderBindingTable, &callableShaderBindingTable,
                                                   extent.width, extent.height, 1);

        // Acquire output image and swapchain image for copying and transition to appropriate layouts accordingly.
        VulkanImageMemoryBarrier::Insert(commandBuffer, m_OutputImage->GetHandle(), subresourceRange, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        VulkanImageMemoryBarrier::Insert(commandBuffer, GetSwapChain().GetImages()[imageIndex], subresourceRange, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // Copy output image into swapchain image.
        VkImageCopy copyRegion = {};
        copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        copyRegion.srcOffset = { 0, 0, 0 };
        copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        copyRegion.dstOffset = { 0, 0, 0 };
        copyRegion.extent = { extent.width, extent.height, 1 };

        vkCmdCopyImage(commandBuffer, m_OutputImage->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, GetSwapChain().GetImages()[imageIndex],
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        VulkanImageMemoryBarrier::Insert(commandBuffer, GetSwapChain().GetImages()[imageIndex], subresourceRange, VK_ACCESS_TRANSFER_WRITE_BIT, 0,
                                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }

    void RaytracingApplication::CreateAccelerationStructures()
    {
        const std::chrono::high_resolution_clock::time_point timer = std::chrono::high_resolution_clock::now();

        SingleTimeCommands::Submit(GetCommandPool(), [this](VkCommandBuffer commandBuffer)
        {
            CreateBottomLevelStructures(commandBuffer);
            CreateTopLevelStructures(commandBuffer);
        });

        m_TopASScratchBuffer.reset();
        m_TopASScratchBufferMemory.reset();
        m_BottomASScratchBuffer.reset();
        m_BottomASScratchBufferMemory.reset();

        const float elapsedTime = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - timer).count();
        std::cout << "Built Acceleration Structures in " << elapsedTime << " seconds.\n";
    }

    void RaytracingApplication::DeleteAccelerationStructures()
    {
        m_TopAccelerationStructures.clear();

        m_InstancesBuffer.reset();
        m_InstancesBufferMemory.reset();
        m_TopASScratchBuffer.reset();
        m_TopASScratchBufferMemory.reset();
        m_TopASBuffer.reset();
        m_TopASBufferMemory.reset();

        m_BottomAccelerationStructures.clear();

        m_BottomASScratchBuffer.reset();
        m_BottomASScratchBufferMemory.reset();
        m_BottomASBuffer.reset();
        m_BottomASBufferMemory.reset();
    }

    void RaytracingApplication::CreateBottomLevelStructures(VkCommandBuffer commandBuffer)
    {
        const Resources::Scene& scene = GetScene();
        const VulkanDebugUtilities& debugUtilities = GetDevice().GetDebugUtilities();

        // Bottom Level AS. Triangles via Vertex Buffers. Procedurals via AABBs.
        uint32_t vertexOffset = 0;
        uint32_t indexOffset = 0;
        uint32_t aabbOffset = 0;

        for (const auto& model : scene.GetModels())
        {
            const uint32_t vertexCount = static_cast<uint32_t>(model.GetNumberOfVertices());
            const uint32_t indexCount = static_cast<uint32_t>(model.GetNumberOfIndices());
            VulkanBottomLevelGeometry geometries;

            model.GetProcedural() ? geometries.AddGeometry_AABB(scene, aabbOffset, 1, true) : geometries.AddGeometry_Triangles(scene, vertexOffset, vertexCount, indexOffset, indexCount, true);

            m_BottomAccelerationStructures.emplace_back(*m_RaytracingCommandList, *m_RaytracingProperties, geometries);

            vertexOffset += vertexCount * sizeof(Resources::Vertex);
            indexOffset += indexCount * sizeof(uint32_t);
            aabbOffset += sizeof(VkAabbPositionsKHR);
        }

        // Allocate the structures memory.
        const VkAccelerationStructureBuildSizesInfoKHR totalMemory = ASUtilities::GetTotalRequirements(m_BottomAccelerationStructures);

        m_BottomASBuffer.reset(new VulkanBuffer(GetDevice(), totalMemory.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT));
        m_BottomASBufferMemory.reset(new VulkanDeviceMemory(m_BottomASBuffer->AllocateMemory(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
        m_BottomASScratchBuffer.reset(new VulkanBuffer(GetDevice(), totalMemory.buildScratchSize, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR));
        m_BottomASScratchBufferMemory.reset(new VulkanDeviceMemory(m_BottomASScratchBuffer->AllocateMemory(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

        debugUtilities.SetObjectName(m_BottomASBuffer->GetHandle(), "BLAS Buffer");
        debugUtilities.SetObjectName(m_BottomASBufferMemory->GetHandle(), "BLAS Memory");
        debugUtilities.SetObjectName(m_BottomASScratchBuffer->GetHandle(), "BLAS Scratch Buffer");
        debugUtilities.SetObjectName(m_BottomASScratchBufferMemory->GetHandle(), "BLAS Scratch Memory");

        // Generate the structures.
        VkDeviceSize resultBufferOffset = 0;
        VkDeviceSize scratchBufferOffset = 0;

        for (size_t i = 0; i != m_BottomAccelerationStructures.size(); ++i)
        {
            m_BottomAccelerationStructures[i].Generate(commandBuffer, *m_BottomASScratchBuffer, scratchBufferOffset, *m_BottomASBuffer, resultBufferOffset);

            resultBufferOffset += m_BottomAccelerationStructures[i].GetBuildSizes().accelerationStructureSize;
            scratchBufferOffset += m_BottomAccelerationStructures[i].GetBuildSizes().buildScratchSize;

            debugUtilities.SetObjectName(m_BottomAccelerationStructures[i].GetHandle(), ("BLAS #" + std::to_string(i)).c_str());
        }
    }

    void RaytracingApplication::CreateTopLevelStructures(VkCommandBuffer commandBuffer)
    {
        const Resources::Scene& scene = GetScene();
        const VulkanDebugUtilities& debugUtilities = GetDevice().GetDebugUtilities();

        // Top level AS.
        std::vector<VkAccelerationStructureInstanceKHR> instances;

        // Hit Group 0 - Triangles
        // Hit Group 1 - Procedurals
        uint32_t instanceID = 0;

        for (const auto& model : scene.GetModels())
        {
            instances.push_back(VulkanTopLevelAS::CreateASInstance(m_BottomAccelerationStructures[instanceID], glm::mat4(1.0f), instanceID, model.GetProcedural() ? 1 : 0));
            instanceID++;
        }

        // Create and copy instances buffer (do it in a seperate one-time synchronous command buffer).
        VulkanBufferUtilities::CreateDeviceBuffer(GetCommandPool(), "TLAS Instances", VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, instances, m_InstancesBuffer, m_InstancesBufferMemory);

        // Memory Barrier for BLAS Builds
        VulkanAccelerationStructure::MemoryBarrier(commandBuffer);

        m_TopAccelerationStructures.emplace_back(*m_RaytracingCommandList, *m_RaytracingProperties, m_InstancesBuffer->GetDeviceAddress(), static_cast<uint32_t>(instances.size()));

        // Allocate the structure memory.
        const VkAccelerationStructureBuildSizesInfoKHR totalMemory = ASUtilities::GetTotalRequirements(m_TopAccelerationStructures);

        m_TopASBuffer.reset(new VulkanBuffer(GetDevice(), totalMemory.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR));
        m_TopASBufferMemory.reset(new VulkanDeviceMemory(m_TopASBuffer->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

        m_TopASScratchBuffer.reset(new VulkanBuffer(GetDevice(), totalMemory.buildScratchSize, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR));
        m_TopASScratchBufferMemory.reset(new VulkanDeviceMemory(m_TopASScratchBuffer->AllocateMemory(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

        debugUtilities.SetObjectName(m_TopASBuffer->GetHandle(), "TLAS Buffer");
        debugUtilities.SetObjectName(m_TopASBufferMemory->GetHandle(), "TLAS Memory");
        debugUtilities.SetObjectName(m_TopASScratchBuffer->GetHandle(), "TLAS Scratch Buffer");
        debugUtilities.SetObjectName(m_TopASScratchBufferMemory->GetHandle(), "TLAS Scratch Memory");
        debugUtilities.SetObjectName(m_InstancesBuffer->GetHandle(), "TLAS Instances Buffer");
        debugUtilities.SetObjectName(m_InstancesBufferMemory->GetHandle(), "TLAS Instances Memory");

        // Generate the structure.
        m_TopAccelerationStructures[0].Generate(commandBuffer, *m_TopASScratchBuffer, 0, *m_TopASBuffer, 0);
        debugUtilities.SetObjectName(m_TopAccelerationStructures[0].GetHandle(), "TLAS");
    }

    void RaytracingApplication::CreateOutputImage()
    {
        const VkExtent2D extent = GetSwapChain().GetExtent();
        const VkFormat format = GetSwapChain().GetFormat();
        const VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL; // We will always go for optimal tiling.

        m_AccumulationImage.reset(new VulkanImage(GetDevice(), extent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT));
        m_AccumulationImageMemory.reset(new VulkanDeviceMemory(m_AccumulationImage->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
        m_AccumulationImageView.reset(new VulkanImageView(GetDevice(), m_AccumulationImage->GetHandle(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT));

        m_OutputImage.reset(new VulkanImage(GetDevice(), extent, format, tiling, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
        m_OutputImageMemory.reset(new VulkanDeviceMemory(m_OutputImage->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
        m_OutputImageView.reset(new VulkanImageView(GetDevice(), m_OutputImage->GetHandle(), format, VK_IMAGE_ASPECT_COLOR_BIT));

        const VulkanDebugUtilities& debugUtilities = GetDevice().GetDebugUtilities();

        debugUtilities.SetObjectName(m_AccumulationImage->GetHandle(), "Accumulation Image");
        debugUtilities.SetObjectName(m_AccumulationImageMemory->GetHandle(), "Accumulation Image Memory");
        debugUtilities.SetObjectName(m_AccumulationImageView->GetHandle(), "Accumulation Image View");

        debugUtilities.SetObjectName(m_OutputImage->GetHandle(), "Output Image");
        debugUtilities.SetObjectName(m_OutputImageMemory->GetHandle(), "Output Image Memory");
        debugUtilities.SetObjectName(m_OutputImageView->GetHandle(), "Output Image View");
    }
}