#include "Core.h"
#include <iostream>

namespace Vulkan
{
    void CheckResult(VkResult result, const char* operation)
    {
        if (result != VK_SUCCESS)
        {
            std::cout << "Failed to perform operation: " << operation << "(" << ToString(result) << ")\n";
        }
        else
        {
            std::cout << "Successfully performed: " << operation << "\n";
        }
    }

    const char* ToString(VkResult result)
    {
        switch (result)
        {
            #define StringResult(result) case VK_##result: return #result
                StringResult(SUCCESS);
                StringResult(NOT_READY);
                StringResult(TIMEOUT);
                StringResult(EVENT_SET);
                StringResult(EVENT_RESET);
                StringResult(INCOMPLETE);
                StringResult(ERROR_OUT_OF_HOST_MEMORY);
                StringResult(ERROR_OUT_OF_DEVICE_MEMORY);
                StringResult(ERROR_INITIALIZATION_FAILED);
                StringResult(ERROR_DEVICE_LOST);
                StringResult(ERROR_MEMORY_MAP_FAILED);
                StringResult(ERROR_LAYER_NOT_PRESENT);
                StringResult(ERROR_EXTENSION_NOT_PRESENT);
                StringResult(ERROR_FEATURE_NOT_PRESENT);
                StringResult(ERROR_INCOMPATIBLE_DRIVER);
                StringResult(ERROR_TOO_MANY_OBJECTS);
                StringResult(ERROR_FORMAT_NOT_SUPPORTED);
                StringResult(ERROR_FRAGMENTED_POOL);
                StringResult(ERROR_UNKNOWN);
                StringResult(ERROR_OUT_OF_POOL_MEMORY);
                StringResult(ERROR_INVALID_EXTERNAL_HANDLE);
                StringResult(ERROR_FRAGMENTATION);
                StringResult(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
                StringResult(ERROR_SURFACE_LOST_KHR);
                StringResult(ERROR_NATIVE_WINDOW_IN_USE_KHR);
                StringResult(SUBOPTIMAL_KHR);
                StringResult(ERROR_OUT_OF_DATE_KHR);
                StringResult(ERROR_INCOMPATIBLE_DISPLAY_KHR);
                StringResult(ERROR_VALIDATION_FAILED_EXT);
                StringResult(ERROR_INVALID_SHADER_NV);
                StringResult(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
                StringResult(ERROR_NOT_PERMITTED_EXT);
                StringResult(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
                StringResult(THREAD_IDLE_KHR);
                StringResult(THREAD_DONE_KHR);
                StringResult(OPERATION_DEFERRED_KHR);
                StringResult(OPERATION_NOT_DEFERRED_KHR);
                StringResult(PIPELINE_COMPILE_REQUIRED_EXT);
            #undef StringResult
        default:
            return "Unknown Error";
        }
    }
}