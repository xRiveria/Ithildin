#include "VulkanRaytracingPipeline.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanSwapChain.h"
#include "Vulkan/VulkanDescriptorBinding.h"
#include "Vulkan/VulkanDescriptorSetLayout.h"
#include "Vulkan/VulkanDescriptorSetManager.h"
#include "Vulkan/VulkanDescriptorSets.h"
#include "Vulkan/VulkanPipelineLayout.h"
#include "Vulkan/VulkanImageView.h"
#include "Vulkan/VulkanShaderModule.h"
#include "Vulkan/VulkanBuffer.h"
#include "VulkanTopLevelAS.h"
#include "VulkanRaytracingCommandList.h"
#include "VulkanBottomLevelAS.h"
#include "Resources/Scene.h"
#include "Resources/UniformBuffer.h"

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
            { 0, 1, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, VK_SHADER_STAGE_RAYGEN_BIT_KHR },
            // Image Accumulation & Output
            { 1, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR },
            { 2, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR },

            // Camera Information & Others
            { 3, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR },

            // Vertex Buffer, Index Buffer, Material buffer, Offset Buffer
            { 4, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR },
            { 5, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR },
            { 6, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR },
            { 7, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR },

            // Textures and Image samplers
            { 8, static_cast<uint32_t>(scene.GetTextureSamplers().size()), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR },

            // Procedural Buffer
            { 9, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_INTERSECTION_BIT_KHR }
        };

        m_DescriptorSetManager.reset(new VulkanDescriptorSetManager(device, descriptorBindings, uniformBuffers.size()));

        VulkanDescriptorSets& descriptorSets = m_DescriptorSetManager->GetDescriptorSets();

        for (uint32_t i = 0; i != swapChain.GetImages().size(); ++i)
        {
            // Top Level Acceleration Structure
            const VkAccelerationStructureKHR accelerationStructureHandle = accelerationStructure.GetHandle();

            VkWriteDescriptorSetAccelerationStructureKHR accelerationStructureInfo = {};
            accelerationStructureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
            accelerationStructureInfo.pNext = nullptr;
            accelerationStructureInfo.accelerationStructureCount = 1;
            accelerationStructureInfo.pAccelerationStructures = &accelerationStructureHandle;

            // Accumation Image
            VkDescriptorImageInfo accumulationImageInfo = {};
            accumulationImageInfo.imageView = accumulationImageView.GetHandle();
            accumulationImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

            // Output Image
            VkDescriptorImageInfo outputImageInfo = {};
            outputImageInfo.imageView = outputImageView.GetHandle();
            outputImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

            // Uniform Buffer
            VkDescriptorBufferInfo uniformBufferInfo = {};
            uniformBufferInfo.buffer = uniformBuffers[i].GetBuffer().GetHandle();
            uniformBufferInfo.range = VK_WHOLE_SIZE;

            // Vertex Buffer
            VkDescriptorBufferInfo vertexBufferInfo = {};
            vertexBufferInfo.buffer = scene.GetVertexBuffer().GetHandle();
            vertexBufferInfo.range = VK_WHOLE_SIZE;

            // Index Buffer
            VkDescriptorBufferInfo indexBufferInfo = {};
            indexBufferInfo.buffer = scene.GetIndexBuffer().GetHandle();
            indexBufferInfo.range = VK_WHOLE_SIZE;

            // Material Buffer
            VkDescriptorBufferInfo materialBufferInfo = {};
            materialBufferInfo.buffer = scene.GetMaterialBuffer().GetHandle();
            materialBufferInfo.range = VK_WHOLE_SIZE;

            // Offsets Buffer
            VkDescriptorBufferInfo offsetsBufferInfo = {};
            offsetsBufferInfo.buffer = scene.GetOffsetBuffer().GetHandle();
            offsetsBufferInfo.range = VK_WHOLE_SIZE;

            // Image and Texture Samplers
            std::vector<VkDescriptorImageInfo> imageInfos(scene.GetTextureSamplers().size());

            for (size_t i = 0; i != imageInfos.size(); ++i)
            {
                VkDescriptorImageInfo& imageInfo = imageInfos[i];
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = scene.GetTextureImageViews()[i];
                imageInfo.sampler = scene.GetTextureSamplers()[i];
            }

            std::vector<VkWriteDescriptorSet> descriptorWrites =
            {
                descriptorSets.Bind(i, 0, accelerationStructureInfo),
                descriptorSets.Bind(i, 1, accumulationImageInfo),
                descriptorSets.Bind(i, 2, outputImageInfo),
                descriptorSets.Bind(i, 3, uniformBufferInfo),
                descriptorSets.Bind(i, 4, vertexBufferInfo),
                descriptorSets.Bind(i, 5, indexBufferInfo),
                descriptorSets.Bind(i, 6, materialBufferInfo),
                descriptorSets.Bind(i, 7, offsetsBufferInfo),
                descriptorSets.Bind(i, 8, *imageInfos.data(), static_cast<uint32_t>(imageInfos.size()))
            };

            // Procedural Buffer (Optional)
            VkDescriptorBufferInfo proceduralBufferInfo = {};

            if (scene.HasProcedurals())
            {
                proceduralBufferInfo.buffer = scene.GetProceduralBuffer().GetHandle();
                proceduralBufferInfo.range = VK_WHOLE_SIZE;

                descriptorWrites.push_back(descriptorSets.Bind(i, 9, proceduralBufferInfo));
            }

            descriptorSets.UpdateDescriptors(i, descriptorWrites);
        }

        m_PipelineLayout.reset(new VulkanPipelineLayout(device, m_DescriptorSetManager->GetDescriptorSetLayout()));

        // Load Shaders
        const VulkanShaderModule rayGenerationShader(device, "../Assets/Shaders/RayTracing.rgen.spv");
        const VulkanShaderModule missShader(device, "../Assets/Shaders/RayTracing.rmiss.spv");
        const VulkanShaderModule closestHitShader(device, "../Assets/Shaders/RayTracing.rchit.spv");
        const VulkanShaderModule proceduralClosestHitShader(device, "../Assets/Shaders/RayTracing.Procedural.rchit.spv");
        const VulkanShaderModule proceduralIntersectionShader(device, "../Assets/Shaders/RayTracing.Procedural.rint.spv");

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages =
        {
            rayGenerationShader.CreateShaderStage(VK_SHADER_STAGE_RAYGEN_BIT_KHR),
            missShader.CreateShaderStage(VK_SHADER_STAGE_MISS_BIT_KHR),
            closestHitShader.CreateShaderStage(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
            proceduralClosestHitShader.CreateShaderStage(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
            proceduralIntersectionShader.CreateShaderStage(VK_SHADER_STAGE_INTERSECTION_BIT_KHR)
        };

        // Shader Groups
        VkRayTracingShaderGroupCreateInfoKHR rayGenerationGroupInfo = {};
        rayGenerationGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        rayGenerationGroupInfo.pNext = nullptr;
        rayGenerationGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        rayGenerationGroupInfo.generalShader = 0;
        rayGenerationGroupInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
        rayGenerationGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
        rayGenerationGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
        m_RayGenerationShaderIndex = 0;

        VkRayTracingShaderGroupCreateInfoKHR missGroupInfo = {};
        missGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        missGroupInfo.pNext = nullptr;
        missGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        missGroupInfo.generalShader = 1;
        missGroupInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
        missGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
        missGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
        m_MissShaderIndex = 1;

        VkRayTracingShaderGroupCreateInfoKHR triangleHitGroupInfo = {};
        triangleHitGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        triangleHitGroupInfo.pNext = nullptr;
        triangleHitGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
        triangleHitGroupInfo.generalShader = VK_SHADER_UNUSED_KHR;
        triangleHitGroupInfo.closestHitShader = 2;
        triangleHitGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
        triangleHitGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
        m_TriangleHitGroupIndex = 2;

        VkRayTracingShaderGroupCreateInfoKHR proceduralHitGroupInfo = {};
        proceduralHitGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        proceduralHitGroupInfo.pNext = nullptr;
        proceduralHitGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
        proceduralHitGroupInfo.generalShader = VK_SHADER_UNUSED_KHR;
        proceduralHitGroupInfo.closestHitShader = 3;
        proceduralHitGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
        proceduralHitGroupInfo.intersectionShader = 4;
        m_ProceduralHitGroupIndex = 3;

        std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups =
        {
            rayGenerationGroupInfo,
            missGroupInfo,
            triangleHitGroupInfo,
            proceduralHitGroupInfo
        };

        // Create Raytracing Pipeline
        VkRayTracingPipelineCreateInfoKHR raytracePipelineInfo = {};
        raytracePipelineInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
        raytracePipelineInfo.pNext = nullptr;
        raytracePipelineInfo.flags = 0;
        raytracePipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        raytracePipelineInfo.pStages = shaderStages.data();
        raytracePipelineInfo.groupCount = static_cast<uint32_t>(groups.size());
        raytracePipelineInfo.pGroups = groups.data();
        raytracePipelineInfo.maxPipelineRayRecursionDepth = 1; // Alter this if we plan to perform recursive raytracing.
        raytracePipelineInfo.layout = m_PipelineLayout->GetHandle();
        raytracePipelineInfo.basePipelineHandle = nullptr;
        raytracePipelineInfo.basePipelineIndex = 0;

        CheckResult(commandList.vkCreateRayTracingPipelinesKHR(device.GetHandle(), nullptr, nullptr, 1, &raytracePipelineInfo, nullptr, &m_Pipeline), "Create Raytracing Pipeline");
    }

    Raytracing::VulkanRaytracingPipeline::~VulkanRaytracingPipeline()
    {
        if (m_Pipeline != nullptr)
        {
            vkDestroyPipeline(m_SwapChain.GetDevice().GetHandle(), m_Pipeline, nullptr);
            m_Pipeline = nullptr;
        }

        m_PipelineLayout.reset();
        m_DescriptorSetManager.reset();
    }

    VkDescriptorSet Raytracing::VulkanRaytracingPipeline::GetDescriptorSet(uint32_t index) const
    {
        return m_DescriptorSetManager->GetDescriptorSets().GetDescriptorSetHandle(index);
    }
}