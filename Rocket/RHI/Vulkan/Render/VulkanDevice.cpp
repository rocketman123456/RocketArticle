#include "Vulkan/Render/VulkanDevice.h"

using namespace Rocket;

VulkanDevice::VulkanDevice(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
	RK_GRAPHICS_ASSERT(physical_device, "Invalid Physical Device");
	this->physical_device = physical_device;
	this->surface = surface;
}

void VulkanDevice::Initialize()
{
	// Store Properties features, limits and properties of the physical device for later use
	// Device properties also contain limits and sparse properties
	vkGetPhysicalDeviceProperties(physical_device, &properties);
	// Features should be checked by the examples before using them
	vkGetPhysicalDeviceFeatures(physical_device, &features);
	// Memory properties are used regularly for creating all kinds of buffers
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
	// Queue family properties, used for setting up requested queues upon device creation
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount, nullptr);
	RK_GRAPHICS_ASSERT(queueFamilyCount > 0, "QueueFamilyCount Error");
	queue_family_properties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount, queue_family_properties.data());
}

void VulkanDevice::Finalize()
{
	if (command_pool) {
		vkDestroyCommandPool(logical_device, command_pool, nullptr);
	}
	if (logical_device) {
		vkDestroyDevice(logical_device, nullptr);
	}
}

uint32_t VulkanDevice::GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound)
{
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			if ((memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
				if (memTypeFound) {
					*memTypeFound = true;
				}
				return i;
			}
		}
		typeBits >>= 1;
	}

	if (memTypeFound) {
		*memTypeFound = false;
		return 0;
	}
	else {
		RK_GRAPHICS_ERROR("Could not find a matching memory type");
	}
}

uint32_t VulkanDevice::GetQueueFamilyIndex(VkQueueFlagBits queueFlags)
{
	// Dedicated queue for compute
	// Try to find a queue family index that supports compute but not graphics
	if (queueFlags & VK_QUEUE_COMPUTE_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++) {
			if ((queue_family_properties[i].queueFlags & queueFlags) && ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
				return i;
				break;
			}
		}
	}

	// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
	for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++) {
		if (queue_family_properties[i].queueFlags & queueFlags) {
			return i;
			break;
		}
	}

	RK_GRAPHICS_ERROR("Could not find a matching queue family index");
}

VkResult VulkanDevice::CreateLogicalDevice(
	VkPhysicalDeviceFeatures enabled_features, 
	std::vector<const char*> enabledExtensions, 
	bool enableValidationLayers, 
	const Vec<const char*>& validationLayers,
	VkQueueFlags requestedQueueTypes)
{
	// Desired queues need to be requested upon logical device creation
	// Due to differing queue family configurations of Vulkan implementations this can be a bit tricky, especially if the application
	// requests different queue types

	Vec<VkDeviceQueueCreateInfo> queueCreateInfos{};

	// Get queue family indices for the requested queue family types
	// Note that the indices may overlap depending on the implementation

	QueueFamilyIndices indices = FindQueueFamilies(physical_device, surface);
	//RK_GRAPHICS_TRACE("FindQueueFamilies: Graphics {}, Compute {}, Present {}", 
	//	indices.graphics_family.value(), 
	//	indices.compute_family.value(),
	//	indices.present_family.value()
	//);
	std::set<uint32_t> uniqueQueueFamilies = { 
		indices.graphics_family.value(),
		indices.compute_family.value(),
		indices.present_family.value() 
	};

	const float defaultQueuePriority(0.0f);
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Graphics queue
	if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT) {
		queue_family_indices.graphics_family = indices.graphics_family.value();
	}
	else {
		queue_family_indices.graphics_family = VK_NULL_HANDLE;
	}

	// Dedicated compute queue
	if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT) {
		queue_family_indices.compute_family = indices.compute_family.value();
	}
	else {
		// Else we use the same queue
		queue_family_indices.compute_family = queue_family_indices.graphics_family;
	}

	// Present queue
	queue_family_indices.present_family = indices.present_family.value();

	// Create the logical device representation
	std::vector<const char*> deviceExtensions(enabledExtensions);
	deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &enabled_features;

	if (deviceExtensions.size() > 0) {
		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	}

	if (enableValidationLayers)
	{
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		deviceCreateInfo.enabledLayerCount = 0;
	}

	VkResult result = vkCreateDevice(physical_device, &deviceCreateInfo, nullptr, &logical_device);

	if (result == VK_SUCCESS) {
		command_pool = CreateCommandPool(queue_family_indices.graphics_family.value());
	}

	this->enabled_features = enabled_features;

	if (queue_family_indices.graphics_family.has_value())
		vkGetDeviceQueue(logical_device, queue_family_indices.graphics_family.value(), 0, &graphics_queue);
	if (queue_family_indices.compute_family.has_value())
		vkGetDeviceQueue(logical_device, queue_family_indices.compute_family.value(), 0, &compute_queue);
	if (queue_family_indices.present_family.has_value())
		vkGetDeviceQueue(logical_device, queue_family_indices.present_family.value(), 0, &present_queue);

	return result;
}

VkResult VulkanDevice::CreateBuffer(
	VkBufferUsageFlags usageFlags, 
	VkMemoryPropertyFlags memoryPropertyFlags, 
	VkDeviceSize size, 
	VkBuffer* buffer, 
	VkDeviceMemory* memory, void* data)
{
	// Create the buffer handle
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.usage = usageFlags;
	bufferCreateInfo.size = size;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VK_CHECK(vkCreateBuffer(logical_device, &bufferCreateInfo, nullptr, buffer));

	// Create the memory backing up the buffer handle
	VkMemoryRequirements memReqs;
	VkMemoryAllocateInfo memAlloc{};
	memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkGetBufferMemoryRequirements(logical_device, *buffer, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	// Find a memory type index that fits the properties of the buffer
	memAlloc.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);
	VK_CHECK(vkAllocateMemory(logical_device, &memAlloc, nullptr, memory));

	// If a pointer to the buffer data has been passed, map the buffer and copy over the data
	if (data != nullptr)
	{
		void* mapped;
		VK_CHECK(vkMapMemory(logical_device, *memory, 0, size, 0, &mapped));
		memcpy(mapped, data, size);
		// If host coherency hasn't been requested, do a manual flush to make writes visible
		if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
		{
			VkMappedMemoryRange mappedRange{};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.memory = *memory;
			mappedRange.offset = 0;
			mappedRange.size = size;
			vkFlushMappedMemoryRanges(logical_device, 1, &mappedRange);
		}
		vkUnmapMemory(logical_device, *memory);
	}

	// Attach the memory to the buffer object
	VK_CHECK(vkBindBufferMemory(logical_device, *buffer, *memory, 0));

	return VK_SUCCESS;
}

VkCommandPool VulkanDevice::CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags)
{
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
	cmdPoolInfo.flags = createFlags;
	VkCommandPool cmdPool;
	VK_CHECK(vkCreateCommandPool(logical_device, &cmdPoolInfo, nullptr, &cmdPool));
	return cmdPool;
}

VkCommandBuffer VulkanDevice::CreateCommandBuffer(VkCommandBufferLevel level, bool begin)
{
	VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
	cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufAllocateInfo.commandPool = command_pool;
	cmdBufAllocateInfo.level = level;
	cmdBufAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuffer;
	VK_CHECK(vkAllocateCommandBuffers(logical_device, &cmdBufAllocateInfo, &cmdBuffer));

	// If requested, also start recording for the new command buffer
	if (begin) {
		VkCommandBufferBeginInfo commandBufferBI{};
		commandBufferBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &commandBufferBI));
	}

	return cmdBuffer;
}

void VulkanDevice::BeginCommandBuffer(VkCommandBuffer commandBuffer)
{
	VkCommandBufferBeginInfo commandBufferBI{};
	commandBufferBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBI));
}

void VulkanDevice::FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free)
{
	VK_CHECK(vkEndCommandBuffer(commandBuffer));

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	// Create fence to ensure that the command buffer has finished executing
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	VkFence fence;
	VK_CHECK(vkCreateFence(logical_device, &fenceInfo, nullptr, &fence));

	// Submit to the queue
	VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, fence));
	// Wait for the fence to signal that command buffer has finished executing
	VK_CHECK(vkWaitForFences(logical_device, 1, &fence, VK_TRUE, 100000000000));

	vkDestroyFence(logical_device, fence, nullptr);

	if (free) {
		vkFreeCommandBuffers(logical_device, command_pool, 1, &commandBuffer);
	}
}
