#include "VulkanInstance.h"
#include "../Core/Window.h"
#include "VulkanUtilities.h"

namespace Vulkan
{
    VulkanInstance::VulkanInstance(const Window& window, const std::vector<const char*>& validationLayers, uint32_t vulkanVersion) 
        : m_Window(window), m_ValidationLayers(validationLayers)
    {
        // Check minimum version.
        QueryVulkanMinimumVersion(vulkanVersion);

        // Get the list of required extensions.
        std::vector<const char*> extensions = window.GetRequiredInstanceExtensions();

        // Check the validation layers and ensure they are suported.
        QueryVulkanValidationLayerSupport(validationLayers);

        if (!validationLayers.empty())
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        // Create the Vulkan Instance
        VkApplicationInfo applicationDescription = {};
        applicationDescription.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationDescription.pApplicationName = "Ithildin Raytracing";
        applicationDescription.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationDescription.pEngineName = "Ithildin";
        applicationDescription.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationDescription.apiVersion = vulkanVersion; // Species the Vulkan version against which your code is written.

        VkInstanceCreateInfo instanceDescription = {};
        instanceDescription.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceDescription.pApplicationInfo = &applicationDescription;
        instanceDescription.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceDescription.ppEnabledExtensionNames = extensions.data();
        instanceDescription.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceDescription.ppEnabledLayerNames = validationLayers.data();

        CheckResult(vkCreateInstance(&instanceDescription, nullptr, &m_Instance), "Create Instance");

        GetVulkanPhysicalDevices();
        GetVulkanLayers();
        GetVulkanExtensions();
    }

    VulkanInstance::~VulkanInstance()
    {
        if (m_Instance != nullptr)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = nullptr;
        }
    }

    void VulkanInstance::GetVulkanExtensions()
    {
        GetEnumerateVector(static_cast<const char*>(nullptr), vkEnumerateInstanceExtensionProperties, m_Extensions, "Query Extensions");
    }

    void VulkanInstance::GetVulkanLayers()
    {
        GetEnumerateVector(vkEnumerateInstanceLayerProperties, m_Layers, "Query Layers");
    }

    void VulkanInstance::GetVulkanPhysicalDevices()
    {
        GetEnumerateVector(m_Instance, vkEnumeratePhysicalDevices, m_PhysicalDevices, "Query Physical Devices");

        if (m_PhysicalDevices.empty())
        {
            std::cout << "No Vulkan physical devices were found.\n";
        }
    }

    void VulkanInstance::QueryVulkanMinimumVersion(uint32_t minimumVersion)
    {
        uint32_t version;
        CheckResult(vkEnumerateInstanceVersion(&version), "Query Instance Version"); // Get current Vulkan API Version

        if (minimumVersion > version)
        {
            std::cout << "Minimum required version not found. (Required: " << minimumVersion << ".) (Found: " << version << ")\n";
        }
    }

    void VulkanInstance::QueryVulkanValidationLayerSupport(const std::vector<const char*>& validationLayers)
    {
        const std::vector<VkLayerProperties> avaliableLayers = GetEnumerateVector(vkEnumerateInstanceLayerProperties, "Query Layers");

        for (const char* layer : validationLayers)
        {
            auto result = std::find_if(avaliableLayers.begin(), avaliableLayers.end(), [layer](const VkLayerProperties& layerProperties)
            {
                 return strcmp(layer, layerProperties.layerName) == 0;
            });

            if (result == avaliableLayers.end())
            {
                std::cout << "The requested validation layer could not be found: " << std::string(layer) << "\n";
            }
        }
    }
}