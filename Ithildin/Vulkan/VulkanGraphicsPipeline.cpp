#include "VulkanGraphicsPipeline.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanDepthBuffer.h"
#include "VulkanDescriptorBinding.h"
#include "VulkanDescriptorSetManager.h"
#include "VulkanBuffer.h"
#include "VulkanPipelineLayout.h"
#include "VulkanShaderModule.h"
#include "VulkanRenderPass.h"
#include "VulkanDescriptorSets.h"
#include "Resources/UniformBuffer.h"
#include "Resources/Vertex.h"
#include "Resources/Scene.h"

namespace Vulkan
{
    VulkanGraphicsPipeline::VulkanGraphicsPipeline(const VulkanSwapChain& swapChain, const VulkanDepthBuffer& depthBuffer, const std::vector<Resources::UniformBuffer>& uniformBuffers, const Resources::Scene& scene, bool isWireFrame)
        : m_SwapChain(swapChain), m_IsWireFrame(isWireFrame)
    {
        const VulkanDevice& device = swapChain.GetDevice();
        const VkVertexInputBindingDescription bindingDescription = Resources::Vertex::GetBindingDescription();
        const std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = Resources::Vertex::GetAttributeDescriptions();
        
        // Our vertex format data.
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo vertexAssemblyInfo = {};
        vertexAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        vertexAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        vertexAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        VkViewport viewportInfo = {};
        viewportInfo.x = 0.0f;
        viewportInfo.y = 0.0f;
        viewportInfo.width = static_cast<float>(swapChain.GetExtent().width);
        viewportInfo.height = static_cast<float>(swapChain.GetExtent().height);
        viewportInfo.minDepth = 0.0f;
        viewportInfo.maxDepth = 1.0f;

        VkRect2D scissorsInfo = {};
        scissorsInfo.offset = { 0, 0 };
        scissorsInfo.extent = swapChain.GetExtent();

        VkPipelineViewportStateCreateInfo viewportStateInfo = {};
        viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.pViewports = &viewportInfo;
        viewportStateInfo.scissorCount = 1;
        viewportStateInfo.pScissors = &scissorsInfo;

        VkPipelineRasterizationStateCreateInfo rasterizerStateInfo = {};
        rasterizerStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerStateInfo.depthClampEnable = VK_FALSE;
        rasterizerStateInfo.rasterizerDiscardEnable = VK_FALSE; // Whether primitives are discarded immediately before the rasterization state. Useful to feed data into buffers.
        rasterizerStateInfo.polygonMode = isWireFrame ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
        rasterizerStateInfo.lineWidth = 1.0f;
        rasterizerStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizerStateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizerStateInfo.depthBiasEnable = VK_FALSE;
        rasterizerStateInfo.depthBiasConstantFactor = 0.0f;
        rasterizerStateInfo.depthBiasSlopeFactor = 0.0f;;
        rasterizerStateInfo.depthBiasClamp = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisamplingInfo = {};
        multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisamplingInfo.sampleShadingEnable = VK_FALSE;
        multisamplingInfo.minSampleShading = 1.0f;
        multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisamplingInfo.pSampleMask = nullptr;
        multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
        multisamplingInfo.alphaToOneEnable = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};
        depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateInfo.depthTestEnable = VK_TRUE;
        depthStencilStateInfo.depthWriteEnable = VK_TRUE;
        depthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateInfo.minDepthBounds = 0.0f;
        depthStencilStateInfo.maxDepthBounds = 1.0f;
        depthStencilStateInfo.stencilTestEnable = VK_FALSE;
        depthStencilStateInfo.front = {};
        depthStencilStateInfo.back = {};

        VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo = {};
        colorBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentInfo.blendEnable = VK_FALSE;
        colorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {};
        colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendStateInfo.attachmentCount = 1;
        colorBlendStateInfo.pAttachments = &colorBlendAttachmentInfo;
        colorBlendStateInfo.blendConstants[0] = 0.0f;
        colorBlendStateInfo.blendConstants[1] = 0.0f;
        colorBlendStateInfo.blendConstants[2] = 0.0f;
        colorBlendStateInfo.blendConstants[3] = 0.0f;

        // Create descriptor pool/sets.
        std::vector<VulkanDescriptorBinding> descriptorBindings =
        {
            { 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT },
            { 1, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT },
            { 2, static_cast<uint32_t>(scene.GetTextureSamplers().size()), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT }
        };

        m_DescriptorSetManager.reset(new VulkanDescriptorSetManager(device, descriptorBindings, uniformBuffers.size()));

        VulkanDescriptorSets& descriptorSets = m_DescriptorSetManager->GetDescriptorSets();

        for (uint32_t i = 0; i != swapChain.GetImages().size(); ++i)
        {
            // Uniform Buffer
            VkDescriptorBufferInfo uniformBufferInfo = {};
            uniformBufferInfo.buffer = uniformBuffers[i].GetBuffer().GetHandle();
            uniformBufferInfo.range = VK_WHOLE_SIZE;

            // Material Buffer
            VkDescriptorBufferInfo materialBufferInfo = {};
            materialBufferInfo.buffer = scene.GetMaterialBuffer().GetHandle();
            materialBufferInfo.range = VK_WHOLE_SIZE;

            // Image and texture samplers.
            std::vector<VkDescriptorImageInfo> imageInfos(scene.GetTextureSamplers().size());

            for (size_t t = 0; t != imageInfos.size(); ++t)
            {
                VkDescriptorImageInfo& imageInfo = imageInfos[t];
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = scene.GetTextureImageViews()[t];
                imageInfo.sampler = scene.GetTextureSamplers()[t];
            }

            const std::vector<VkWriteDescriptorSet> descriptorWrites =
            {
                descriptorSets.Bind(i, 0, uniformBufferInfo),  // 1
                descriptorSets.Bind(i, 1, materialBufferInfo), // 1
                descriptorSets.Bind(i, 2, *imageInfos.data(), static_cast<uint32_t>(imageInfos.size()))
            };

            // Binds our resources with the descriptor sets, bound later with the pipeline layout with vkCmdBindDescriptorSets.
            descriptorSets.UpdateDescriptors(i, descriptorWrites);
        }

        // Create pipeline layout and our render pass.
        m_PipelineLayout.reset(new VulkanPipelineLayout(device, m_DescriptorSetManager->GetDescriptorSetLayout()));
        m_RenderPass.reset(new VulkanRenderPass(swapChain, depthBuffer, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_LOAD_OP_CLEAR));

        // Load Shaders
        const VulkanShaderModule vertexShader(device, "../Assets/Shaders/Graphics.vert.spv");
        const VulkanShaderModule fragmentShader(device, "../Assets/Shaders/Graphics.frag.spv");

        VkPipelineShaderStageCreateInfo shaderStages[] =
        {
            vertexShader.CreateShaderStage(VK_SHADER_STAGE_VERTEX_BIT),
            fragmentShader.CreateShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT)
        };

        // Create Graphics Pipeline Object
        VkGraphicsPipelineCreateInfo pipelineCreationInfo = {};
        pipelineCreationInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreationInfo.stageCount = 2;
        pipelineCreationInfo.pStages = shaderStages;
        pipelineCreationInfo.pVertexInputState = &vertexInputInfo;
        pipelineCreationInfo.pInputAssemblyState = &vertexAssemblyInfo;
        pipelineCreationInfo.pViewportState = &viewportStateInfo;
        pipelineCreationInfo.pRasterizationState = &rasterizerStateInfo;
        pipelineCreationInfo.pMultisampleState = &multisamplingInfo;
        pipelineCreationInfo.pDepthStencilState = &depthStencilStateInfo;
        pipelineCreationInfo.pColorBlendState = &colorBlendStateInfo;
        pipelineCreationInfo.pDynamicState = nullptr;
        pipelineCreationInfo.basePipelineHandle = nullptr;
        pipelineCreationInfo.basePipelineIndex = -1;
        pipelineCreationInfo.layout = m_PipelineLayout->GetHandle();
        pipelineCreationInfo.renderPass = m_RenderPass->GetHandle();
        pipelineCreationInfo.subpass = 0;

        CheckResult(vkCreateGraphicsPipelines(device.GetHandle(), nullptr, 1, &pipelineCreationInfo, nullptr, &m_Pipeline), "Graphics Pipeline Creation");
    }
    
    VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
    {
        if (m_Pipeline != nullptr)
        {
            vkDestroyPipeline(m_SwapChain.GetDevice().GetHandle(), m_Pipeline, nullptr);
            m_Pipeline = nullptr;
        }

        m_RenderPass.reset();
        m_PipelineLayout.reset();
        m_DescriptorSetManager.reset();
    }

    VkDescriptorSet VulkanGraphicsPipeline::GetDescriptorSet(uint32_t index) const
    {
        return m_DescriptorSetManager->GetDescriptorSets().GetDescriptorSetHandle(index);
    }
}