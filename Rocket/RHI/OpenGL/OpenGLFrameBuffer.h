#pragma once
#include "Render/DrawBasic/FrameBuffer.h"

namespace Rocket
{
	class OpenGLFrameBuffer : implements FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FramebufferSpec& spec);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();

		void Bind(FRAME_BIND_MODE mode) final;
		void Unbind() final;

		void Resize(uint32_t width, uint32_t height) final;

		uint32_t GetColorAttachmentRendererID(uint32_t index) const final 
		{ 
			RK_CORE_ASSERT(index < m_ColorAttachments.size(), "GetColorAttachmentRendererID index out of range");
			return m_ColorAttachments[index];
		}
		uint32_t GetDepthAttachmentRendererID() const final { return m_DepthAttachment; }
		const FramebufferSpec& GetSpecification() const final { return m_Specification; }
	private:
		uint32_t m_RendererID = 0;
		Vec<uint32_t> m_ColorSpecifications = {};
		Vec<uint32_t> m_ColorAttachments = {};
		uint32_t m_DepthAttachment = 0;
		FramebufferSpec m_Specification;
	};
}