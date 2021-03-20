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

		const Vec<Ref<VertexBuffer>>& GetVertexBuffers() const final { return vertex_buffers_; }
		const Ref<IndexBuffer>& GetIndexBuffer() const final { return index_buffer_;  }

	private:
		uint32_t renderer_id_;
		uint32_t vertex_buffer_index_ = 0;
		Vec<Ref<VertexBuffer>> vertex_buffers_;
		Ref<IndexBuffer> index_buffer_;
	};
}