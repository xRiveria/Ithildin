#include "Core/Window.h"
#include "Vulkan/VulkanApplication.h"
#include "Vulkan/VulkanUtilities.h"

void SetVulkanDevice(Raytracing::VulkanApplication& application);

int main(int argc, int argv[])
{
    const Raytracing::WindowSettings windowSettings
    {
        "Ithildin - Raytracing (x64 Vulkan)",   // Title
        1280,                                   // Width
        720,                                    // Height
        false,                                  // Is Cursor Disabled
        false,                                  // Is Fullscreen
        true                                    // Is Resizable
    };

    /// Create Raytracer Application.
    /// Print SDK information
    /// Print Instance Information
    /// Print Layers Information
    /// Print Vulkan Devices
    /// Set Device
    /// Print Swapchain Information
    /// Run

    Raytracing::VulkanApplication application(windowSettings, VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR, true);
    SetVulkanDevice(application);

    while (true)
    {

    }
}

void SetVulkanDevice(Raytracing::VulkanApplication& application)
{
    const std::vector<VkPhysicalDevice>& physicalDevices = application.GetPhysicalDevices();
    const std::vector<VkPhysicalDevice>::const_iterator result = std::find_if(physicalDevices.begin(), physicalDevices.end(), [](const VkPhysicalDevice& device)
    {
        // We want a device with geometry shader support.
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        if (!deviceFeatures.geometryShader)
        {
            return false;
        }

        // We want a device that supports the raytracing extension.
        const std::vector<VkExtensionProperties> extensions = Raytracing::GetEnumerateVector(device, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties, "Enumerate Raytracing Extensions");
        const std::vector<VkExtensionProperties>::const_iterator hasRaytracingSupport = std::find_if(extensions.begin(), extensions.end(), [](const VkExtensionProperties& extension)
        {
            return strcmp(extension.extensionName, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) == 0;
        });

        if (hasRaytracingSupport == extensions.end())
        {
            return false;
        }
        else
        {
            std::cout << "Successfully found Raytracing extension.\n";
        }

        // We want a device with a graphics queue.
        const auto queueFamilies = Raytracing::GetEnumerateVector(device, vkGetPhysicalDeviceQueueFamilyProperties, "Querying Queue Families");
        const auto hasGraphicsQueue = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties& queueFamily)
        {
            return queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        });

        return hasGraphicsQueue != queueFamilies.end();
    });

    if (result == physicalDevices.end())
    {
        std::runtime_error("A suitable physical device could not be found.\n");
    }

    VkPhysicalDeviceProperties2 deviceProperties = {};
    deviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(*result, &deviceProperties);

    std::cout << "Setting Device [" << deviceProperties.properties.deviceName << "]\n";

    application.SetPhysicalDevice(*result);
}