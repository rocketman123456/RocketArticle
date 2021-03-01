#pragma once
//#include "Render/DrawBasic/BufferLayout.h"
//#include "Render/DrawBasic/VertexArray.h"
//#include "Render/DrawBasic/VertexBuffer.h"
//#include "Render/DrawBasic/IndexBuffer.h"
#include "Vulkan/Render/VulkanFunction.h"
#include "Vulkan/Render/VulkanDevice.h"

#include <Vulkan/vulkan.h>

namespace Rocket
{
	struct VulkanBufferStruct
	{
		VkDevice device;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkDescriptorBufferInfo descriptor;
		VkDeviceSize size;
		int32_t count = 0;
		void* mapped = nullptr;

		void Create(
			Ref<VulkanDevice> device,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryPropertyFlags,
			VkDeviceSize size, bool map = true);
		void Finalize();
		void Map();
		void Unmap();
		void Flush(VkDeviceSize size = VK_WHOLE_SIZE);
	};
}