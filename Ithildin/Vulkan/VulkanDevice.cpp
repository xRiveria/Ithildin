#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanInstance.h"
#include "VulkanUtilities.h"
#include <string>
#include <iostream>
#include <set>

namespace Raytracing
{
    namespace QueueUtilities
    {
        std::vector<VkQueueFamilyProperties>::const_iterator FindQueue(const std::vector<VkQueueFamilyProperties>& queueFamilies,
                                                                       const std::string name, const VkQueueFlags requiredBits, const VkQueueFlags excludedBits)
        {   
            const auto queueFamily = std::find_if(queueFamilies.begin(), queueFamilies.end(), [requiredBits, excludedBits](const VkQueueFamilyProperties& queueFamily)
            {
                return queueFamily.queueCount > 0 && queueFamily.queueFlags & requiredBits && !(queueFamily.queueFlags & excludedBits);
            });

            if (queueFamily == queueFamilies.end())
            {
                std::runtime_error("Found no matching queue families for: " + name + " Queue.\n");
            }

            std::cout << "Successfully found queue family for: " + name + " Queue.\n";
            return queueFamily;
        }
    }

    VulkanDevice::VulkanDevice(VkPhysicalDevice physicalDevice, const VulkanSurface& surface, const std::vector<const char*>& requiredExtensions, const VkPhysicalDeviceFeatures& deviceFeatures, const void* nextDeviceFeatures)
                             : m_PhysicalDevice(physicalDevice), m_Surface(surface), m_DebugUtilities(surface.GetInstance().GetHandle())
    {
        QueryRequiredExtensions(physicalDevice, requiredExtensions);

        const std::vector<VkQueueFamilyProperties> queueFamilies = GetEnumerateVector(physicalDevice, vkGetPhysicalDeviceQueueFamilyProperties, "Enumerate Queue Families");

        // Find the graphics queue.
        const std::vector<VkQueueFamilyProperties>::const_iterator graphicsFamily = QueueUtilities::FindQueue(queueFamilies, "Graphics", VK_QUEUE_GRAPHICS_BIT, 0);
        const std::vector<VkQueueFamilyProperties>::const_iterator computeFamily  = QueueUtilities::FindQueue(queueFamilies, "Compute", VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);
        const std::vector<VkQueueFamilyProperties>::const_iterator transferFamily = QueueUtilities::FindQueue(queueFamilies, "Transfer", VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

        // Find the presentation queue (usually the same as the graphics queue).
        const auto presentationFamily = std::find_if(queueFamilies.begin(), queueFamilies.end(), [&](const VkQueueFamilyProperties& queueFamily)
        {
            VkBool32 presentationSupport = false;
            const uint32_t i = static_cast<uint32_t>(&*queueFamilies.cbegin() - &queueFamily);
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface.GetHandle(), &presentationSupport);

            return queueFamily.queueCount > 0 && presentationSupport;
        });

        if (presentationFamily == queueFamilies.end())
        {
            std::runtime_error("No presentation queues were found.");
        }

        m_QueueGraphicsFamilyIndex = static_cast<uint32_t>(graphicsFamily - queueFamilies.begin());
        m_QueueComputeFamilyIndex  = static_cast<uint32_t>(computeFamily - queueFamilies.begin());
        m_QueuePresentFamilyIndex  = static_cast<uint32_t>(presentationFamily - queueFamilies.begin());
        m_QueueTransferFamilyIndex = static_cast<uint32_t>(transferFamily - queueFamilies.begin());

        // Queues can be the same. Set ensures that none of the values within are repeated and that they are unique.
        const std::set<uint32_t> uniqueQueueFamilies =
        {
            m_QueueGraphicsFamilyIndex,
            m_QueueComputeFamilyIndex,
            m_QueuePresentFamilyIndex,
            m_QueueTransferFamilyIndex
        };

        // Create queues.
        float queuePriority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        // Create Device
        VkDeviceCreateInfo deviceCreationInfo = {};
        deviceCreationInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreationInfo.pNext = nextDeviceFeatures;
        deviceCreationInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreationInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreationInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreationInfo.enabledLayerCount = static_cast<uint32_t>(surface.GetInstance().GetValidationLayers().size());
        deviceCreationInfo.ppEnabledLayerNames = surface.GetInstance().GetValidationLayers().data();
        deviceCreationInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        deviceCreationInfo.ppEnabledExtensionNames = requiredExtensions.data();

        CheckResult(vkCreateDevice(physicalDevice, &deviceCreationInfo, nullptr, &m_Device), "Create Logical Device");

        m_DebugUtilities.SetDevice(m_Device);

        vkGetDeviceQueue(m_Device, m_QueueGraphicsFamilyIndex, 0, &m_QueueGraphics);
        vkGetDeviceQueue(m_Device, m_QueueComputeFamilyIndex, 0, &m_QueueCompute);
        vkGetDeviceQueue(m_Device, m_QueuePresentFamilyIndex, 0, &m_QueuePresent);
        vkGetDeviceQueue(m_Device, m_QueueTransferFamilyIndex, 0, &m_QueueTransfer);
    }

    VulkanDevice::~VulkanDevice()
    {
        if (m_Device != nullptr)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device = nullptr;
        }
    }

    void VulkanDevice::WaitIdle() const
    {
        // Waits on the host for the completion of outstanding queue operations for all queues on a given logical device.
        CheckResult(vkDeviceWaitIdle(m_Device), "Waiting for Device Idle.\n");
    }

    void VulkanDevice::QueryRequiredExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions) const
    {
        const std::vector<VkExtensionProperties> avaliableExtensions = GetEnumerateVector(physicalDevice, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties, "Query Device Extensions");
        std::set<std::string> _requiredExtensions(requiredExtensions.begin(), requiredExtensions.end());

        for (const VkExtensionProperties& extension : avaliableExtensions)
        {
            _requiredExtensions.erase(extension.extensionName); // Set erase is guarenteed to end in a valid state, even if the requested object to delete does not exist.
        }

        if (!_requiredExtensions.empty())
        {
            bool first = true;
            std::string extensions;

            for (const std::string& extension : _requiredExtensions)
            {
                if (!first)
                {
                    extensions += ", ";
                }

                extensions += extension;
                first = false;
            }

            std::runtime_error("Missing required extensions: " + extensions);
        }
    }
}