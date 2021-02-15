#pragma once
#include "Render/DrawBasic/BufferLayout.h"
#include "Render/DrawBasic/VertexArray.h"
#include "Render/DrawBasic/VertexBuffer.h"
#include "Render/DrawBasic/IndexBuffer.h"

#include <Vulkan/vulkan.h>

namespace Rocket
{
    class VulkanVertexBuffer : implements VertexBuffer
    {
    public:
        VulkanVertexBuffer();
		virtual ~VulkanVertexBuffer();

        void Bind() const final;
		void Unbind() const final;

		void SetData(const void* data, uint32_t size) final;

		const BufferLayout& GetLayout() const final;
		void SetLayout(const BufferLayout& layout) final;
    private:
        BufferLayout m_Layout;
    };

    class VulkanIndexBuffer : implements IndexBuffer
    {
    public:
        VulkanIndexBuffer();
        virtual ~VulkanIndexBuffer();

		void Bind() const final;
		void Unbind() const final;

		uint32_t GetCount() const final;
    };
}