#pragma once
#include "../Core/Core.h"
#include <vector>

namespace Raytracing
{
    class Window;

    class VulkanInstance final
    {
    public:
        VulkanInstance(const Window& window, const std::vector<const char*>& validationLayers, uint32_t vulkanVersion);
        ~VulkanInstance();

        const Window& GetWindow() const { return m_Window; }

        const std::vector<const char*>& GetValidationLayers() const { return m_ValidationLayers; }
        const std::vector<VkExtensionProperties>& GetExtensions() const { return m_Extensions; }
        const std::vector<VkLayerProperties>& GetLayers() const { return m_Layers; }
        const std::vector<VkPhysicalDevice>& GetPhysicalDevices() const { return m_PhysicalDevices; }

    private:
        void GetVulkanExtensions();
        void GetVulkanLayers();
        void GetVulkanPhysicalDevices();

        static void QueryVulkanMinimumVersion(uint32_t minimumVersion);
        static void QueryVulkanValidationLayerSupport(const std::vector<const char*>& validationLayers);

    private:
        const std::vector<const char*> m_ValidationLayers;
        std::vector<VkExtensionProperties> m_Extensions;
        std::vector<VkLayerProperties> m_Layers;
        std::vector<VkPhysicalDevice> m_PhysicalDevices;

        const Window& m_Window;
        VULKAN_HANDLE(VkInstance, m_Instance)
    };
}