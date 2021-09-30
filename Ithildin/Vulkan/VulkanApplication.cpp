#include "VulkanApplication.h"
#include "VulkanInstance.h"
#include "VulkanDebugMessenger.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "../Core/Window.h"

namespace Raytracing
{
    VulkanApplication::VulkanApplication(const WindowSettings& windowSettings, VkPresentModeKHR presentationMode, bool enableValidationLayers)
        : m_PresentMode(presentationMode)
    {
        const std::vector<const char*> validationLayers = enableValidationLayers ? std::vector<const char*> { "VK_LAYER_KHRONOS_validation" } : std::vector<const char*>();

        m_Window.reset(new Window(windowSettings));
        m_Instance.reset(new VulkanInstance(*m_Window, validationLayers, VK_API_VERSION_1_2));
        m_DebugUtilities.reset(enableValidationLayers ? new VulkanDebugMessenger(*m_Instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) : nullptr);
        m_Surface.reset(new VulkanSurface(*m_Instance));
    }

    void VulkanApplication::SetPhysicalDevice(VkPhysicalDevice physicalDevice)
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

        // Create SwapChain and Command Buffers.

    }

    void VulkanApplication::SetPhysicalDevice(VkPhysicalDevice physicalDevice, std::vector<const char*>& requiredExtensions, VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures)
    {
        m_Device.reset(new VulkanDevice(physicalDevice, *m_Surface, requiredExtensions, deviceFeatures, nextDeviceFeatures));
    }

    void VulkanApplication::CreateSwapChain()
    {
    }

    VulkanApplication::~VulkanApplication()
    {
        // Reverse creation order.
        m_Surface.reset();
        m_DebugUtilities.reset();
        m_Instance.reset();
        m_Window.reset();
    }

    const std::vector<VkExtensionProperties>& VulkanApplication::GetExtensions() const
    {
        return m_Instance->GetExtensions();
    }

    const std::vector<VkLayerProperties>& VulkanApplication::GetLayers() const
    {
        return m_Instance->GetLayers();
    }

    const std::vector<VkPhysicalDevice>& VulkanApplication::GetPhysicalDevices() const
    {
        return m_Instance->GetPhysicalDevices();
    }
}