#pragma once
#include "Render/DrawBasic/FrameBuffer.h"

namespace Rocket
{
	class OpenGLFrameBuffer : implements Framebuffer
	{
	public:
		OpenGLFrameBuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();

		void Bind() final;
		void Unbind() final;

		void Resize(uint32_t width, uint32_t height) final;

		uint32_t GetColorAttachmentRendererID() const final { return m_ColorAttachment; }
		uint32_t GetDepthAttachmentRendererID() const final { return m_DepthAttachment; }

		const FramebufferSpecification& GetSpecification() const final { return m_Specification; }

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0;
		uint32_t m_DepthAttachment = 0;
		uint32_t m_DepthStencilAttachment = 0;
		FramebufferSpecification m_Specification;
	};
}