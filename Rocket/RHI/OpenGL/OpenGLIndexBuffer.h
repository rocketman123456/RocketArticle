#pragma once
#include "Render/DrawBasic/IndexBuffer.h"

namespace Rocket
{
	class OpenGLIndexBuffer : implements IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override { return count_; }

	private:
		uint32_t renderer_id_;
		uint32_t count_;
	};
}