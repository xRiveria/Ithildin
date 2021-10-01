#include "VulkanSemaphore.h"
#include "VulkanDevice.h"

namespace Raytracing
{
    VulkanSemaphore::VulkanSemaphore(const VulkanDevice& device) : m_Device(device)
    {
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        CheckResult(vkCreateSemaphore(m_Device.GetHandle(), &semaphoreInfo, nullptr, &m_Semaphore), "Semaphore Creation");
    }

    VulkanSemaphore::VulkanSemaphore(VulkanSemaphore&& otherSemaphore) noexcept : m_Device(otherSemaphore.m_Device), m_Semaphore(otherSemaphore.m_Semaphore)
    {
        otherSemaphore.m_Semaphore = nullptr;
    }

    VulkanSemaphore::~VulkanSemaphore()
    {
        if (m_Semaphore != nullptr)
        {
            vkDestroySemaphore(m_Device.GetHandle(), m_Semaphore, nullptr);
            m_Semaphore = nullptr;
        }
    }
}