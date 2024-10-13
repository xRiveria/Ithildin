#pragma once
#include "../Core/Core.h"

namespace Vulkan
{
    class VulkanInstance;
    class Window;

    class VulkanSurface final
    {
    public:
        explicit VulkanSurface(const VulkanInstance& instance);
        ~VulkanSurface();

        const VulkanInstance& GetInstance() const { return m_Instance; }

    private:
        const VulkanInstance& m_Instance;
        VULKAN_HANDLE(VkSurfaceKHR, m_Surface)
    };
}