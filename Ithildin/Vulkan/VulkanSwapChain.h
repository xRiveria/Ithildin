#pragma once
#include "../Core/Core.h"
#include <memory>
#include <vector>

namespace Raytracing
{
    class VulkanDevice;
    class VulkanImageView;
    class Window;

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR m_Capabilities;
        std::vector<VkSurfaceFormatKHR> m_Formats;
        std::vector<VkPresentModeKHR> m_PresentationModes;
    };

    class VulkanSwapChain final
    {
    public:
        VulkanSwapChain(const VulkanDevice& device, VkPresentModeKHR requestedPresentationMode);
        ~VulkanSwapChain();

        VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
        const VulkanDevice& GetDevice() const { return m_Device; }
        uint32_t GetMinimumImageCount() const { return m_MinimumImageCount; }
        const std::vector<VkImage>& GetImages() const { return m_Images; }
        const std::vector<std::unique_ptr<VulkanImageView>>& GetImageViews() const { return m_ImageViews; }
        const VkExtent2D& GetExtent() const { return m_Extent; }
        VkFormat GetFormat() const { return m_Format; }
        VkPresentModeKHR GetPresentationMode() const { return m_PresentationMode; }

    private:
        static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        static VkSurfaceFormatKHR SelectSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
        static VkPresentModeKHR SelectSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes, VkPresentModeKHR requestedPresentationMode);
        static VkExtent2D SelectSwapChainExtent(const Window& window, const VkSurfaceCapabilitiesKHR& capabilities);
        static uint32_t SelectImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

    private:
        uint32_t m_MinimumImageCount;
        VkPresentModeKHR m_PresentationMode;
        VkFormat m_Format;
        VkExtent2D m_Extent;
        std::vector<VkImage> m_Images;
        std::vector<std::unique_ptr<VulkanImageView>> m_ImageViews;

        const VkPhysicalDevice m_PhysicalDevice;
        const VulkanDevice& m_Device;

        VULKAN_HANDLE(VkSwapchainKHR, m_SwapChain)
    };
}