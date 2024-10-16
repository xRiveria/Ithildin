#include "VulkanDebugUtilities.h"
#include <stdexcept>

namespace Vulkan
{
    VulkanDebugUtilities::VulkanDebugUtilities(VkInstance vulkanInstance) 
        : vkSetDebugUtilsObjectNameEXT(reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(vulkanInstance, "vkSetDebugUtilsObjectNameEXT")))
    {
        #ifdef _DEBUG
        if (vkSetDebugUtilsObjectNameEXT == nullptr)
        {
            std::runtime_error("Failed to obtain address of vkSetDebugUtilsObjectNameEXT.\n");
        }
        #endif
    }
}