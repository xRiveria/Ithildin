#pragma once

#define NOMINMAX
#define GLFW_INCLUDE_NONE //Explictly prevents GLFW from including any OpenGL headers.
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VULKAN_HANDLE(VulkanHandleType, Name)           \
public:                                                 \
    VulkanHandleType GetHandle() const { return Name; } \
private:                                                \
    VulkanHandleType Name;

namespace Vulkan
{
    void CheckResult(VkResult result, const char* operation);
    const char* ToString(VkResult result);
}