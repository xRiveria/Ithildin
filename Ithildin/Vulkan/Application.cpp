#include "Application.h"
#include "VulkanInstance.h"
#include "VulkanDebugMessenger.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanImageView.h"
#include "VulkanCommandPool.h"
#include "VulkanSwapChain.h"
#include "VulkanBuffer.h"
#include "VulkanDepthBuffer.h"
#include "VulkanFence.h"
#include "VulkanSemaphore.h"
#include "VulkanPipelineLayout.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommandBuffers.h"
#include "VulkanRenderPass.h"
#include "Resources/UniformBuffer.h"
#include "Resources/Scene.h"
#include "Resources/Model.h"
#include "Resources/Texture.h"
#include "../Core/Window.h"
#include <string>

namespace Vulkan
{
    Application::Application(const WindowSettings& windowSettings, VkPresentModeKHR presentationMode, bool enableValidationLayers)
        : m_PresentationMode(presentationMode)
    {
        const std::vector<const char*> validationLayers = enableValidationLayers ? std::vector<const char*> { "VK_LAYER_KHRONOS_validation" } : std::vector<const char*>();

        m_Window.reset(new Window(windowSettings));
        m_Instance.reset(new VulkanInstance(*m_Window, validationLayers, VK_API_VERSION_1_2));
        m_DebugMessenger.reset(enableValidationLayers ? new VulkanDebugMessenger(*m_Instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) : nullptr);
        m_Surface.reset(new VulkanSurface(*m_Instance));
    }

    Application::~Application()
    {
        DeleteSwapChain();

        // Reverse creation order.
        m_CommandPool.reset();
        m_Device.reset();
        m_Surface.reset();
        m_DebugMessenger.reset();
        m_Instance.reset();
        m_Window.reset();
    }

    void Application::SetPhysicalDevice(VkPhysicalDevice physicalDevice)
    {
        if (m_Device)
        {
            std::logic_error("A physical device has already been set.\n");
        }

        std::vector<const char*> requiredExtensions =
        {
            // VK_KHR_swapchain
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        VkPhysicalDeviceFeatures deviceFeatures = {};

        SetPhysicalDevice(physicalDevice, requiredExtensions, deviceFeatures, nullptr);
        OnDeviceSet();

        // Create SwapChain and Command Buffers.
        CreateSwapChain();
    }

    void Application::OnUpdate()
    {
        if (!m_Device)
        {
            std::logic_error("Physical device has not been set. Is there any error?\n");
        }

        m_Window->DrawFrame = [this]()                                                                { DrawFrame(); };
        m_Window->OnKey = [this](const int key, const int scanCode, const int action, const int mods) { OnKey(key, scanCode, action, mods); };
        m_Window->OnCursorMoved = [this](const double xPosition, const double yPosition)              { OnCursorMoved(xPosition, yPosition); };
        m_Window->OnMouseButton = [this](const int button, const int action, const int mods)          { OnMouseButton(button, action, mods); };
        m_Window->OnScroll = [this](const double xOffset, const double yOffset)                       { OnScroll(xOffset, yOffset); };
        m_Window->Update();
        m_Device->WaitIdle(); // Ensure that all queued operations in a single frame are complete.
    }

    const Resources::Scene& Application::GetScene() const
    {
        return *m_Scene;
    }

    void Application::DrawFrame()
    {
        constexpr auto noTimeout = std::numeric_limits<uint64_t>::max();

        VulkanFence& inFlightFence = m_InFlightFences[m_CurrentFrame];
        const VkSemaphore imageAvaliableSemaphore = m_ImageAvaliableSemaphores[m_CurrentFrame].GetHandle();
        const VkSemaphore renderFinishedSemaphore = m_RenderFinishedSemaphores[m_CurrentFrame].GetHandle();

        inFlightFence.Wait(noTimeout);

        uint32_t imageIndex;

        // Signals semaphore once image is acquired.
        VkResult result = vkAcquireNextImageKHR(m_Device->GetHandle(), m_SwapChain->GetHandle(), noTimeout, imageAvaliableSemaphore, nullptr, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_IsWireframe != m_GraphicsPipeline->IsWireFrame())
        {
            RecreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            std::runtime_error("Failed to acquire next image for SwapChain.\n");
        }

        // Remember that we have a command buffer for each image in the swapchain. We will pass in its index to retrieve the corresponding buffer.
        const VkCommandBuffer commandBuffer = m_CommandBuffers->BeginRecording(imageIndex);
        Render(commandBuffer, imageIndex);
        m_CommandBuffers->EndRecording(imageIndex);

        UpdateUniformBuffer(imageIndex);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        
        VkCommandBuffer commandBuffers[] { commandBuffer };
        VkSemaphore waitSemaphores[]{ imageAvaliableSemaphore }; // Wait for this semaphore to be signalled.
        VkPipelineStageFlags waitStages[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // At this stage.
        VkSemaphore signalSemaphores[] = { renderFinishedSemaphore }; // Alert this when done for presentation below.

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffers;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        inFlightFence.Reset(); 

        // Put fence here for signalling once execution is complete.
        CheckResult(vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, inFlightFence.GetHandle()), (std::string("Submitting Queue Operation (Drawing) For Frame :") + std::to_string(m_CurrentFrame)).c_str());
        
        VkSwapchainKHR swapChains[] = { m_SwapChain->GetHandle() };

        VkPresentInfoKHR presentationInfo = {};
        presentationInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentationInfo.waitSemaphoreCount = 1;
        presentationInfo.pWaitSemaphores = signalSemaphores; // Before presenting, wait for this semaphore to be signalled (signalled when image is acquired).
        presentationInfo.swapchainCount = 1;
        presentationInfo.pSwapchains = swapChains;
        presentationInfo.pImageIndices = &imageIndex;
        presentationInfo.pResults = nullptr;

        result = vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentationInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            RecreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS)
        {
            std::runtime_error("Failed to present next Swapchain Image.\n");
        }

        // Advance to the next frame.
        m_CurrentFrame = (m_CurrentFrame + 1) % m_InFlightFences.size();
    }

    void Application::Render(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_GraphicsPipeline->GetRenderPass().GetHandle();
        renderPassInfo.framebuffer = m_SwapChainFramebuffers[imageIndex].GetHandle();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_SwapChain->GetExtent();
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        // Begin a new render pass. 
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); // Contents of the subpass will be recorded inline in the primary command buffer, and secondary command buffers must be not be executed within the subpass.

        const Resources::Scene& scene = GetScene();

        VkDescriptorSet descriptorSets[] = { m_GraphicsPipeline->GetDescriptorSet(imageIndex) };
        VkBuffer vertexBuffers[] = { m_Scene->GetVertexBuffer().GetHandle() };
        const VkBuffer indexBuffer = m_Scene->GetIndexBuffer().GetHandle();
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->GetHandle());
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->GetPipelineLayout().GetHandle(), 0, 1, descriptorSets, 0, nullptr);
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        uint32_t vertexOffset = 0;
        uint32_t indexOffset = 0;

        for (const Resources::Model& model : scene.GetModels())
        {
            const uint32_t vertexCount = static_cast<uint32_t>(model.GetNumberOfVertices());
            const uint32_t indexCount = static_cast<uint32_t>(model.GetNumberOfIndices());

            vkCmdDrawIndexed(commandBuffer, indexCount, 1, indexOffset, vertexOffset, 0);

            vertexOffset += vertexCount;
            indexOffset += indexCount;
        }

        vkCmdEndRenderPass(commandBuffer);
    }

    void Application::SetPhysicalDevice(VkPhysicalDevice physicalDevice, std::vector<const char*>& requiredExtensions, VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures)
    {
        m_Device.reset(new VulkanDevice(physicalDevice, *m_Surface, requiredExtensions, deviceFeatures, nextDeviceFeatures));
        m_CommandPool.reset(new VulkanCommandPool(*m_Device, m_Device->GetGraphicsQueueFamilyIndex(), true));
    }

    void Application::DeleteSwapChain()
    {
        m_CommandBuffers.reset();
        m_SwapChainFramebuffers.clear();
        m_GraphicsPipeline.reset();
        m_UniformBuffers.clear();
        m_InFlightFences.clear();
        m_RenderFinishedSemaphores.clear();
        m_ImageAvaliableSemaphores.clear();
        m_DepthBuffer.reset();
        m_SwapChain.reset();
    }

    void Application::CreateSwapChain()
    {
        // Wait until the window is visible.
        while (m_Window->IsMinimized())
        {
            m_Window->WaitForEvents();
        }

        m_SwapChain.reset(new VulkanSwapChain(*m_Device, m_PresentationMode));
        m_DepthBuffer.reset(new VulkanDepthBuffer(*m_CommandPool, m_SwapChain->GetExtent()));

        for (size_t i = 0; i != m_SwapChain->GetImageViews().size(); ++i)
        {
            m_ImageAvaliableSemaphores.emplace_back(*m_Device);
            m_RenderFinishedSemaphores.emplace_back(*m_Device);
            m_InFlightFences.emplace_back(*m_Device, true);
            m_UniformBuffers.emplace_back(*m_Device);
        }

        m_GraphicsPipeline.reset(new VulkanGraphicsPipeline(*m_SwapChain, *m_DepthBuffer, m_UniformBuffers, GetScene(), false));

        for (const auto& imageView : m_SwapChain->GetImageViews())
        {
            m_SwapChainFramebuffers.emplace_back(*imageView, m_GraphicsPipeline->GetRenderPass());
        }

        m_CommandBuffers.reset(new VulkanCommandBuffers(*m_CommandPool, static_cast<uint32_t>(m_SwapChainFramebuffers.size())));
    }

    void Application::OnDeviceSet()
    {
        LoadScene(0);
    }

    void Application::LoadScene(uint32_t sceneIndex)
    {
        auto [models, textures] = SceneList::s_AllScenes[sceneIndex].second(m_CameraInitialState);

        // If there are no textures, add a dummy one. It makes the pipeline setup a lot easier.
        if (textures.empty())
        {
            textures.push_back(Resources::Texture::LoadTexture("../Assets/Textures/White.png", Vulkan::SamplerConfiguration()));
        }

        m_Scene.reset(new Resources::Scene(*m_CommandPool, std::move(models), std::move(textures), true));
    }


    void Application::UpdateUniformBuffer(uint32_t imageIndex)
    {
        // m_UniformBuffers[imageIndex].SetValue(GetUniformBufferObject(m_SwapChain->GetExtent()));
    }

    void Application::RecreateSwapChain()
    {
        // Await operations to complete before deletion.
        m_Device->WaitIdle();
        DeleteSwapChain();
        CreateSwapChain();
    }

    const std::vector<VkExtensionProperties>& Application::GetExtensions() const
    {
        return m_Instance->GetExtensions();
    }

    const std::vector<VkLayerProperties>& Application::GetLayers() const
    {
        return m_Instance->GetLayers();
    }

    const std::vector<VkPhysicalDevice>& Application::GetPhysicalDevices() const
    {
        return m_Instance->GetPhysicalDevices();
    }
}