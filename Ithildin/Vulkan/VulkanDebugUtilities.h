#pragma once
#include "../Core/Core.h"

namespace Raytracing
{
    class VulkanDebugUtilities final
    {
	public:
        explicit VulkanDebugUtilities(VkInstance vulkanInstance);
        ~VulkanDebugUtilities() = default;

        void SetDevice(VkDevice device)
        {
            m_Device = device;
        }

		void SetObjectName(const VkAccelerationStructureKHR& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR); }
		void SetObjectName(const VkBuffer& object, const char* name) const				     { SetObjectName(object, name, VK_OBJECT_TYPE_BUFFER); }
		void SetObjectName(const VkCommandBuffer& object, const char* name) const	         { SetObjectName(object, name, VK_OBJECT_TYPE_COMMAND_BUFFER); }
		void SetObjectName(const VkDescriptorSet& object, const char* name) const	         { SetObjectName(object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET); }
		void SetObjectName(const VkDescriptorSetLayout& object, const char* name) const		 { SetObjectName(object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT); }
		void SetObjectName(const VkDeviceMemory& object, const char* name) const			 { SetObjectName(object, name, VK_OBJECT_TYPE_DEVICE_MEMORY); }
		void SetObjectName(const VkFramebuffer& object, const char* name) const				 { SetObjectName(object, name, VK_OBJECT_TYPE_FRAMEBUFFER); }
		void SetObjectName(const VkImage& object, const char* name) const					 { SetObjectName(object, name, VK_OBJECT_TYPE_IMAGE); }
		void SetObjectName(const VkImageView& object, const char* name) const				 { SetObjectName(object, name, VK_OBJECT_TYPE_IMAGE_VIEW); }
		void SetObjectName(const VkPipeline& object, const char* name) const				 { SetObjectName(object, name, VK_OBJECT_TYPE_PIPELINE); }
		void SetObjectName(const VkQueue& object, const char* name) const				     { SetObjectName(object, name, VK_OBJECT_TYPE_QUEUE); }
		void SetObjectName(const VkRenderPass& object, const char* name) const			     { SetObjectName(object, name, VK_OBJECT_TYPE_RENDER_PASS); }
		void SetObjectName(const VkSemaphore& object, const char* name) const				 { SetObjectName(object, name, VK_OBJECT_TYPE_SEMAPHORE); }
		void SetObjectName(const VkShaderModule& object, const char* name) const			 { SetObjectName(object, name, VK_OBJECT_TYPE_SHADER_MODULE); }
		void SetObjectName(const VkSwapchainKHR& object, const char* name) const			 { SetObjectName(object, name, VK_OBJECT_TYPE_SWAPCHAIN_KHR); }

	private:
		template <typename T>
		void SetObjectName(const T& object, const char* name, VkObjectType objectType) const
		{
			#ifndef NDEBUG
			VkDebugUtilsObjectNameInfoEXT namingDescription = {};
			namingDescription.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			namingDescription.pNext = nullptr;
			namingDescription.objectHandle = reinterpret_cast<const uint64_t&>(object);
			namingDescription.objectType = objectType;
			namingDescription.pObjectName = name;

			CheckResult(vkSetDebugUtilsObjectNameEXT(m_Device, &namingDescription), "Set Object Name");
			#endif
		}

    private:
        const PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
        VkDevice m_Device;
    };
}