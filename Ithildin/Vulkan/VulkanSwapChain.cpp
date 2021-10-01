#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanInstance.h"
#include "VulkanUtilities.h"
#include "VulkanDebugUtilities.h"
#include "VulkanImageView.h"
#include "../Core/Window.h"
#include <stdexcept>
#include <string>

namespace Raytracing
{
    VulkanSwapChain::VulkanSwapChain(const VulkanDevice& device, VkPresentModeKHR requestedPresentationMode)
        : m_PhysicalDevice(device.GetPhysicalDevice()), m_Device(device)
    {
        const SwapChainSupportDetails supportDetails = QuerySwapChainSupport(device.GetPhysicalDevice(), device.GetSurface().GetHandle());
        if (supportDetails.m_Formats.empty() || supportDetails.m_PresentationModes.empty())
        {
            std::runtime_error("Empty SwapChain Support.\n");
        }

        const VulkanSurface& surface = device.GetSurface();
        const Window& window = surface.GetInstance().GetWindow();

        const VkSurfaceFormatKHR surfaceFormat = SelectSwapChainSurfaceFormat(supportDetails.m_Formats);
        const VkPresentModeKHR actualPresentationMode = SelectSwapChainPresentationMode(supportDetails.m_PresentationModes, requestedPresentationMode);
        const VkExtent2D extent = SelectSwapChainExtent(window, supportDetails.m_Capabilities);
        const uint32_t imageCount = SelectImageCount(supportDetails.m_Capabilities);

        VkSwapchainCreateInfoKHR swapchainCreationInfo = {};
        swapchainCreationInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreationInfo.surface = surface.GetHandle();
        swapchainCreationInfo.minImageCount = imageCount;
        swapchainCreationInfo.imageFormat = surfaceFormat.format;
        swapchainCreationInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreationInfo.imageExtent = extent;
        swapchainCreationInfo.imageArrayLayers = 1; // Always 1 unless we're developing a stereoscopic application.
        swapchainCreationInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; // TRANSFER_DST_BIT: We will use memory operations to transfer our rendered image to the swapchain.
        swapchainCreationInfo.preTransform = supportDetails.m_Capabilities.currentTransform;
        swapchainCreationInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreationInfo.presentMode = actualPresentationMode;
        swapchainCreationInfo.clipped = VK_TRUE;
        swapchainCreationInfo.oldSwapchain = nullptr;

        if (device.GetGraphicsQueueFamilyIndex() != device.GetPresentQueueFamilyIndex())
        {
            uint32_t queueFamilyIndices[] = { device.GetGraphicsQueueFamilyIndex(), device.GetPresentQueueFamilyIndex() };
            swapchainCreationInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreationInfo.queueFamilyIndexCount = 2;
            swapchainCreationInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            swapchainCreationInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreationInfo.queueFamilyIndexCount = 0; // Optional
            swapchainCreationInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        CheckResult(vkCreateSwapchainKHR(device.GetHandle(), &swapchainCreationInfo, nullptr, &m_SwapChain), "SwapChain Creation");

        m_MinimumImageCount = std::max(2u, supportDetails.m_Capabilities.minImageCount);
        m_PresentationMode = actualPresentationMode;
        m_Format = surfaceFormat.format;
        m_Extent = extent;
        m_Images = GetEnumerateVector(device.GetHandle(), m_SwapChain, vkGetSwapchainImagesKHR, "SwapChain Images Retrieval");
        m_ImageViews.reserve(m_Images.size());

        for (const VkImage image : m_Images)
        {
            m_ImageViews.push_back(std::make_unique<VulkanImageView>(device, image, m_Format, VK_IMAGE_ASPECT_COLOR_BIT));
        }

        const VulkanDebugUtilities& debugUtilities = device.GetDebugUtilities();

        for (size_t i = 0; i != m_Images.size(); ++i)
        {
            debugUtilities.SetObjectName(m_Images[i], ("SwapChain Image #" + std::to_string(i)).c_str());
            debugUtilities.SetObjectName(m_ImageViews[i]->GetHandle(), ("SwapChain Image View #" + std::to_string(i)).c_str());
        }
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        m_ImageViews.clear();

        // Images are destroyed alongside the swapchain.
        if (m_SwapChain != nullptr)
        {
            vkDestroySwapchainKHR(m_Device.GetHandle(), m_SwapChain, nullptr);
            m_SwapChain = nullptr;
        }
    }

    SwapChainSupportDetails VulkanSwapChain::QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        SwapChainSupportDetails supportDetails;
        
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &supportDetails.m_Capabilities);
        supportDetails.m_Formats = GetEnumerateVector(physicalDevice, surface, vkGetPhysicalDeviceSurfaceFormatsKHR, "Query Surface Formats");
        supportDetails.m_PresentationModes = GetEnumerateVector(physicalDevice, surface, vkGetPhysicalDeviceSurfacePresentModesKHR, "Query Presentation Modes");

        return supportDetails;
    }

    VkSurfaceFormatKHR VulkanSwapChain::SelectSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
    {
        if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
        {
            return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }; // Support for SRGB.
        }

        for (const VkSurfaceFormatKHR& format : formats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return format;
            }
        }

        std::runtime_error("Found no suitable surface formats.\n");
        return { VK_FORMAT_UNDEFINED, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
    }

    VkPresentModeKHR VulkanSwapChain::SelectSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes, VkPresentModeKHR requestedPresentationMode)
    {
        switch (requestedPresentationMode)
        {
            case VK_PRESENT_MODE_IMMEDIATE_KHR:
            case VK_PRESENT_MODE_MAILBOX_KHR:
            case VK_PRESENT_MODE_FIFO_KHR:
            case VK_PRESENT_MODE_FIFO_RELAXED_KHR:

                if (std::find(presentationModes.begin(), presentationModes.end(), requestedPresentationMode) != presentationModes.end())
                {
                    return requestedPresentationMode;
                }

                break;

            default: 
                std::runtime_error("Unknown Presentation Mode.\n");
        }

        // Fallback
        return VK_PRESENT_MODE_FIFO_KHR; // Always guarenteed to be present.
    }

    VkExtent2D VulkanSwapChain::SelectSwapChainExtent(const Window& window, const VkSurfaceCapabilitiesKHR& capabilities)
    {
        /*
            Vulkan tells us to match the resolution of the window by setting the width and height in the currentExtent member of the swapchain creation struct.
            However, some window managers to allow us to differ here and this is indicated by setting the width and height in currentExtent to a special value, 
            the maximum value of uint32_t. In that case, we will pick the resolution that best matches the window within the minImageExtent and maxImageExtent bounds.
        */
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }

        VkExtent2D actualExtent = window.GetFramebufferSize();

        // Pick whichever best matches the window. This will literally clamp our width between the minimum and maximum extents supported by the implementation.
        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }

    uint32_t VulkanSwapChain::SelectImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        // The implementation specifies the minimum amount of images to function properly and we will try to have one more than that to properly implement triple buffering.
        // Else, we can just rely on VK_PRESENT_MODE_MAILBOX_KHR with two buffers.
        uint32_t imageCount = std::max(2u, capabilities.minImageCount); // +1 Front Buffer

        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }

        return imageCount;
    }
}