#include "Application.h"

namespace Vulkan::Raytracing
{
    Raytracing::Application::Application(const WindowSettings& windowSettings, VkPresentModeKHR requestedPresentationMode, bool enabledValidationLayers)
                           : Vulkan::Application(windowSettings, requestedPresentationMode, enabledValidationLayers)
    {
    }
}