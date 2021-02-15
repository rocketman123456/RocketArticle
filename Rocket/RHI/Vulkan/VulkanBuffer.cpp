#include "Vulkan/VulkanBuffer.h"

using namespace Rocket;

VulkanVertexBuffer::VulkanVertexBuffer()
{
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
}

void VulkanVertexBuffer::Bind() const
{
}

void VulkanVertexBuffer::Unbind() const
{
}

void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
{
}

const BufferLayout& VulkanVertexBuffer::GetLayout() const
{
    return m_Layout;
}

void VulkanVertexBuffer::SetLayout(const BufferLayout& layout)
{
    m_Layout = layout;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------

VulkanIndexBuffer::VulkanIndexBuffer()
{
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
}

void VulkanIndexBuffer::Bind() const
{
}

void VulkanIndexBuffer::Unbind() const
{
}

uint32_t VulkanIndexBuffer::GetCount() const
{
    return 0;
}