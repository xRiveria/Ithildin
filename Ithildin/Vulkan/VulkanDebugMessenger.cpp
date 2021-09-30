#include "VulkanDebugMessenger.h"
#include "VulkanInstance.h"
#include <iostream>

namespace Raytracing
{
    namespace DebugUtilities
    {
        const char* ObjectTypeToString(const VkObjectType objectType)
        {
            switch (objectType)
            {
                #define ObjectTypeToString(objectType) case VK_OBJECT_TYPE_##objectType: return #objectType
                ObjectTypeToString(UNKNOWN);
                ObjectTypeToString(INSTANCE);
                ObjectTypeToString(PHYSICAL_DEVICE);
                ObjectTypeToString(DEVICE);
                ObjectTypeToString(QUEUE);
                ObjectTypeToString(SEMAPHORE);
                ObjectTypeToString(COMMAND_BUFFER);
                ObjectTypeToString(FENCE);
                ObjectTypeToString(DEVICE_MEMORY);
                ObjectTypeToString(BUFFER);
                ObjectTypeToString(IMAGE);
                ObjectTypeToString(EVENT);
                ObjectTypeToString(QUERY_POOL);
                ObjectTypeToString(BUFFER_VIEW);
                ObjectTypeToString(IMAGE_VIEW);
                ObjectTypeToString(SHADER_MODULE);
                ObjectTypeToString(PIPELINE_CACHE);
                ObjectTypeToString(PIPELINE_LAYOUT);
                ObjectTypeToString(RENDER_PASS);
                ObjectTypeToString(PIPELINE);
                ObjectTypeToString(DESCRIPTOR_SET_LAYOUT);
                ObjectTypeToString(SAMPLER);
                ObjectTypeToString(DESCRIPTOR_POOL);
                ObjectTypeToString(DESCRIPTOR_SET);
                ObjectTypeToString(FRAMEBUFFER);
                ObjectTypeToString(COMMAND_POOL);
                ObjectTypeToString(SAMPLER_YCBCR_CONVERSION);
                ObjectTypeToString(DESCRIPTOR_UPDATE_TEMPLATE);
                ObjectTypeToString(SURFACE_KHR);
                ObjectTypeToString(SWAPCHAIN_KHR);
                ObjectTypeToString(DISPLAY_KHR);
                ObjectTypeToString(DISPLAY_MODE_KHR);
                ObjectTypeToString(DEBUG_REPORT_CALLBACK_EXT);
                ObjectTypeToString(DEBUG_UTILS_MESSENGER_EXT);
                ObjectTypeToString(ACCELERATION_STRUCTURE_KHR);
                ObjectTypeToString(VALIDATION_CACHE_EXT);
                ObjectTypeToString(PERFORMANCE_CONFIGURATION_INTEL);
                ObjectTypeToString(DEFERRED_OPERATION_KHR);
                ObjectTypeToString(INDIRECT_COMMANDS_LAYOUT_NV);
                #undef ObjectTypeToString
                default: return "Unknown";
            }
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                           const VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                           const VkDebugUtilsMessengerCallbackDataEXT* const pCallbackData,
                                                           void* const pUserData)
        {
            switch (messageSeverity)
            {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    std::cerr << "VERBOSE: ";
                    break;

                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    std::cerr << "INFO: ";
                    break;

                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    std::cerr << "WARNING: ";
                    break;

                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    std::cerr << "ERROR: ";
                    break;

                default:
                    std::cerr << "UNKNOWN: ";
            }

            switch (messageType)
            {
                case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                    std::cerr << "GENERAL: ";
                    break;

                case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                    std::cerr << "VALIDATION: ";
                    break;

                case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                    std::cerr << "PERFORMANCE: ";
                    break;

                default:
                    std::cerr << "UNKNOWN: ";
            }

            std::cerr << pCallbackData->pMessage;

            if (pCallbackData->objectCount > 0 && messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            {
                std::cerr << "\n\n Objects (" << pCallbackData->objectCount << "):\n";

                for (uint32_t i = 0; i != pCallbackData->objectCount; ++i)
                {
                    const VkDebugUtilsObjectNameInfoEXT object = pCallbackData->pObjects[i];
                    std::cerr
                        << "  - Object[" << i << "]: "
                        << "Type: " << ObjectTypeToString(object.objectType) << ", "
                        << "Handle: " << reinterpret_cast<void*>(object.objectHandle) << ", "
                        << "Name: '" << (object.pObjectName ? object.pObjectName : "") << "'"
                        << "\n";
                }
            }

            std::cerr << std::endl; // Flush message to output.

            return VK_FALSE;
        }

        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback)
        {
            const PFN_vkCreateDebugUtilsMessengerEXT Function = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
            return (Function != nullptr) ? Function(instance, pCreateInfo, pAllocator, pCallback) : VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator)
        {
            const PFN_vkDestroyDebugUtilsMessengerEXT Function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
            if (Function != nullptr)
            {
                Function(instance, callback, pAllocator);
            }
        }
    }

    VulkanDebugMessenger::VulkanDebugMessenger(const VulkanInstance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold)
        : m_Instance(instance), m_Threshold(threshold)
    {
        if (m_Instance.GetValidationLayers().empty()) // If validation layers are empty.
        {
            return;
        }

        VkDebugUtilsMessageSeverityFlagsEXT severity = 0;

        switch (threshold)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                break;

            default:
                std::cout << "Invalid threshold.\n";
        }

        VkDebugUtilsMessengerCreateInfoEXT debugCreationInfo = {};
        debugCreationInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreationInfo.messageSeverity = severity;
        debugCreationInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreationInfo.pfnUserCallback = DebugUtilities::VulkanDebugCallback;
        debugCreationInfo.pUserData = nullptr;

        CheckResult(DebugUtilities::CreateDebugUtilsMessengerEXT(m_Instance.GetHandle(), &debugCreationInfo, nullptr, &m_Messenger), "Setup Vulkan Debug Callback");
    }

    VulkanDebugMessenger::~VulkanDebugMessenger()
    {
        if (m_Messenger != nullptr)
        {
            DebugUtilities::DestroyDebugUtilsMessengerEXT(m_Instance.GetHandle(), m_Messenger, nullptr);
            m_Messenger = nullptr;
        }
    }
}