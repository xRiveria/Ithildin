#pragma once
#include "Core/Core.h"
#include <memory>

namespace Vulkan
{
    class VulkanCommandPool;
    class VulkanDepthBuffer;
    class VulkanDescriptorPool;
    class VulkanFramebuffer;
    class VulkanRenderPass;
    class VulkanSwapChain;
}

struct UserSettings;

struct Statistics
{
    VkExtent2D m_FramebufferSize;
    float m_FrameRate;
    float m_RayRate;
    uint32_t m_TotalSamples;
};

class Editor final
{
public:
    Editor(Vulkan::VulkanCommandPool& commandPool, const Vulkan::VulkanSwapChain& swapChain, const Vulkan::VulkanDepthBuffer& depthBuffer, UserSettings& userSettings);
    ~Editor();

    void Render(VkCommandBuffer commandBuffer, const Vulkan::VulkanFramebuffer& frameBuffer, const Statistics& statistics);

    bool WantsToCaptureKeyboard() const;
    bool WantsToCaptureMouse() const;

    UserSettings& GetSettings() { return m_UserSettings; }

private:
    void DrawSettings();
    void DrawOverlay(const Statistics& statistics);

private:
    std::unique_ptr<Vulkan::VulkanDescriptorPool> m_DescriptorPool;
    std::unique_ptr<Vulkan::VulkanRenderPass> m_RenderPass;
    UserSettings& m_UserSettings;
};