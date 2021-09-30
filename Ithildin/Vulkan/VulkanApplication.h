#pragma once
#include <vector>
#include "../Core/WindowUtilities.h"
#include "../Core/Core.h"

namespace Raytracing
{
    class VulkanApplication
    {
    public:
        virtual ~VulkanApplication();

        const std::vector<VkExtensionProperties>& GetExtensions() const;
        const std::vector<VkLayerProperties>& GetLayers() const;
        const std::vector<VkPhysicalDevice>& GetPhysicalDevices() const;

    public:
        VulkanApplication(const WindowSettings& windowSettings, VkPresentModeKHR presentationMode, bool enableValidationLayers);
        void SetPhysicalDevice(VkPhysicalDevice physicalDevice);

    protected:
        virtual void SetPhysicalDevice(VkPhysicalDevice physicalDevice, std::vector<const char*>& requiredExtensions, VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures);
        virtual void CreateSwapChain();

    private:
        // Properties
        bool m_IsWireframe = false;
        const VkPresentModeKHR m_PresentMode;

    private:
        std::unique_ptr<class Window> m_Window;
        std::unique_ptr<class VulkanInstance> m_Instance;
        std::unique_ptr<class VulkanDebugMessenger> m_DebugUtilities;
        std::unique_ptr<class VulkanSurface> m_Surface;
        std::unique_ptr<class VulkanDevice> m_Device;
    };
}