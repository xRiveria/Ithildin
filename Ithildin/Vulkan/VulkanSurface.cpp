#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "../Core/Window.h"

namespace Raytracing
{
    VulkanSurface::VulkanSurface(const VulkanInstance& instance) : m_Instance(instance)
    {
        // Creates a Vulkan rendering surface for the specified window. Ensure that the window was created with the client API hint set to GLFW_NO_API.
        CheckResult(glfwCreateWindowSurface(instance.GetHandle(), instance.GetWindow().GetWindowHandle(), nullptr, &m_Surface), "Create Window Surface.");
    }

    VulkanSurface::~VulkanSurface()
    {
        if (m_Surface != nullptr)
        {
            vkDestroySurfaceKHR(m_Instance.GetHandle(), m_Surface, nullptr);
            m_Surface = nullptr;
        }
    }
}