#pragma once
#include "Core/Core.h"

namespace Rocket
{
	ENUM(FrameFormat) { FRAMEBUFFER=0, READ_FRAMEBUFFER, DRAW_FRAMEBUFFER, RENDERBUFFER };
	ENUM(FrameAttachment) { COLOR_ATTACHMENT = 0, DEPTH_ATTACHMENT, DEPTH_STENCIL_ATTACHMENT };

	struct FramebufferSpecification
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;

		FrameFormat Format = FrameFormat::FRAMEBUFFER;
		Vec<FrameAttachment> Attachment = { 
			FrameAttachment::COLOR_ATTACHMENT,
			FrameAttachment::DEPTH_ATTACHMENT,
			FrameAttachment::DEPTH_STENCIL_ATTACHMENT,
		};
	};

	Interface Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
	};
}