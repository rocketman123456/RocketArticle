#pragma once
#include "Core/Core.h"

namespace Rocket
{
	ENUM(FRAME_TEXTURE_FORMAT) { NONE = 0, RGB8, RGBA8, RGBA16, RGB16F, RGBA16F, DEPTH24, DEPTH24STENCIL8 };
	ENUM(FRAME_BIND_MODE) { FRAMEBUFFER = 0, READ_FRAMEBUFFER, DRAW_FRAMEBUFFER };

	struct FramebufferSpec
	{
		uint32_t ColorWidth = 0;
		uint32_t ColorHeight = 0;
		uint32_t DepthWidth = 0;
		uint32_t DepthHeight = 0;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;

		Vec<FRAME_TEXTURE_FORMAT> ColorAttachment = {};
		FRAME_TEXTURE_FORMAT DepthAttachment = FRAME_TEXTURE_FORMAT::NONE;
	};

	Interface FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind(FRAME_BIND_MODE mode) = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index) const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual const FramebufferSpec& GetSpecification() const = 0;
	};
}