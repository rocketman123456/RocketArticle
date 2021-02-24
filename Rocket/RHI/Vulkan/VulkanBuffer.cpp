#include "Vulkan/VulkanBuffer.h"

using namespace Rocket;

void VulkanBufferStruct::Create(
	Ref<VulkanDevice> device,
	VkBufferUsageFlags usageFlags,
	VkMemoryPropertyFlags memoryPropertyFlags,
	VkDeviceSize size, bool map)
{
	RK_GRAPHICS_TRACE("VulkanBufferStruct Create");
	this->device = device->logicalDevice;
	this->size = size;
	device->CreateBuffer(usageFlags, memoryPropertyFlags, size, &buffer, &memory);
	descriptor = { buffer, 0, size };
	if (map) {
		VK_CHECK(vkMapMemory(device->logicalDevice, memory, 0, size, 0, &mapped));
	}
}

void VulkanBufferStruct::Finalize()
{
	if (mapped) {
		Unmap();
	}
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, memory, nullptr);
	buffer = VK_NULL_HANDLE;
	memory = VK_NULL_HANDLE;
}

void VulkanBufferStruct::Map()
{
	VK_CHECK(vkMapMemory(device, memory, 0, size, 0, &mapped));
}

void VulkanBufferStruct::Unmap()
{
	if (mapped)
	{
		vkUnmapMemory(device, memory);
		mapped = nullptr;
	}
}

void VulkanBufferStruct::Flush(VkDeviceSize size)
{
	VkMappedMemoryRange mappedRange{};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = memory;
	mappedRange.size = this->size;
	VK_CHECK(vkFlushMappedMemoryRanges(device, 1, &mappedRange));
}