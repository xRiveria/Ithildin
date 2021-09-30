#include "VulkanApplication.h"
#include "VulkanInstance.h"
#include "../Core/Window.h"

namespace Raytracing
{
    VulkanApplication::~VulkanApplication()
    {
    }

    VulkanApplication::VulkanApplication(const WindowSettings& windowSettings, VkPresentModeKHR presentationMode, bool enableValidationLayers)
        : m_PresentMode(presentationMode)
    {
        const std::vector<const char*> validationLayers = enableValidationLayers ? std::vector<const char*> { "VK_LAYER_KHRONOS_validation" } : std::vector<const char*>();

        m_Window.reset(new Window(windowSettings));
        m_Instance.reset(new VulkanInstance(*m_Window, validationLayers, VK_API_VERSION_1_2));
        /// Debug Utils Messenger
        /// Surface
    }
}