#pragma once
#include "Render/DrawBasic/VertexArray.h"

namespace Rocket
{
	class OpenGLVertexArray : implements VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		void Bind() const final;
		void Unbind() const final;

		void AddVertexBuffer(const Ref<VertexBuffer>&vertexBuffer) final;
		void SetIndexBuffer(const Ref<IndexBuffer>&indexBuffer) final;

		const Vec<Ref<VertexBuffer>>& GetVertexBuffers() const final { return m_VertexBuffers; }
		const Ref<IndexBuffer>& GetIndexBuffer() const final { return m_IndexBuffer;  }

	private:
		uint32_t m_RendererID;
		uint32_t m_VertexBufferIndex = 0;
		Vec<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}