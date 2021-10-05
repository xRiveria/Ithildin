#pragma once
#include <vector>
#include "../Core/WindowUtilities.h"
#include "../Core/Core.h"
#include "SceneList.h"

namespace Resources
{
    class Scene;
    class UniformBufferObject;
    class UniformBuffer;
}

namespace Vulkan
{
    class VulkanSwapChain;
    class Window;
    class VulkanDevice;
    class VulkanCommandPool;

    class Application
    {
    public:
        virtual ~Application();

        const std::vector<VkExtensionProperties>& GetExtensions() const;
        const std::vector<VkLayerProperties>& GetLayers() const;
        const std::vector<VkPhysicalDevice>& GetPhysicalDevices() const;

        const VulkanSwapChain& GetSwapChain() const { return *m_SwapChain; }
        const Window& GetWindow() const { return *m_Window; }
        bool HasSwapChain() const { return m_SwapChain.get(); }

        void SetPhysicalDevice(VkPhysicalDevice physicalDevice);
        void OnUpdate();

    public:
        Application(const WindowSettings& windowSettings, VkPresentModeKHR presentationMode, bool enableValidationLayers);
        virtual const Resources::Scene& GetScene() const = 0;

    protected:
        virtual void DrawFrame();
        virtual void Render(VkCommandBuffer commandBuffer, uint32_t imageIndex);

        virtual void SetPhysicalDevice(VkPhysicalDevice physicalDevice, std::vector<const char*>& requiredExtensions, VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures);
        virtual void CreateSwapChain();
        virtual void DeleteSwapChain();
        virtual void OnDeviceSet() {}
        const VulkanDevice& GetDevice() const { return *m_Device; }
        const std::vector<Resources::UniformBuffer>& GetUniformBuffers() const { return m_UniformBuffers; }
        VulkanCommandPool& GetCommandPool() const { return *m_CommandPool; };

        // Input
        virtual void OnKey(int key, int scanCode, int action, int mods) { }
        virtual void OnCursorMoved(double xPosition, double yPosition) { }
        virtual void OnMouseButton(int button, int action, int mods) { }
        virtual void OnScroll(double xOffset, double yOffset) { }

        virtual Resources::UniformBufferObject GetUniformBufferObject(VkExtent2D extent) const = 0;

    private:
        void UpdateUniformBuffer(uint32_t imageIndex);
        void RecreateSwapChain();

    private:
        // Properties
        bool m_IsWireframe = false;
        const VkPresentModeKHR m_PresentationMode;

    private:        
        std::unique_ptr<class Window> m_Window;
        std::unique_ptr<class VulkanInstance> m_Instance;
        std::unique_ptr<class VulkanDebugMessenger> m_DebugMessenger;
        std::unique_ptr<class VulkanSurface> m_Surface;
        std::unique_ptr<class VulkanDevice> m_Device;
        std::unique_ptr<class VulkanCommandPool> m_CommandPool;
        std::unique_ptr<class VulkanSwapChain> m_SwapChain;
        std::unique_ptr<class VulkanDepthBuffer> m_DepthBuffer;
        std::unique_ptr<class VulkanGraphicsPipeline> m_GraphicsPipeline;
        std::vector<class VulkanFramebuffer> m_SwapChainFramebuffers;
        std::unique_ptr<class VulkanCommandBuffers> m_CommandBuffers;

        std::vector<Resources::UniformBuffer> m_UniformBuffers;
        std::vector<class VulkanFence> m_InFlightFences;
        std::vector<class VulkanSemaphore> m_ImageAvaliableSemaphores;
        std::vector<class VulkanSemaphore> m_RenderFinishedSemaphores;

        size_t m_CurrentFrame = 0;
    };
}