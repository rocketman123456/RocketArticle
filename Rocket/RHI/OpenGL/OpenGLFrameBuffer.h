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
			RK_CORE_ASSERT(index < color_attachments_.size(), "GetColorAttachmentRendererID index out of range");
			return color_attachments_[index];
		}
		uint32_t GetDepthAttachmentRendererID() const final { return depth_attachment_; }
		const FramebufferSpec& GetSpecification() const final { return specification_; }
	private:
		uint32_t renderer_id_ = 0;
		Vec<uint32_t> color_specifications_ = {};
		Vec<uint32_t> color_attachments_ = {};
		uint32_t depth_attachment_ = 0;
		FramebufferSpec specification_;
	};
}