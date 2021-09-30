#include "Core/Window.h"
#include "Vulkan/VulkanApplication.h"

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

    while (true)
    {

    }
}