#pragma once
#include "Core/Core.h"

namespace Rocket
{
	ENUM(FrameBufferTextureFormat) { NONE = 0, RGB8, RGBA8, RGBA16, RGB16F, RGBA16F, DEPTH24, DEPTH24STENCIL8 };
	ENUM(FrameBufferBindMode) { FRAMEBUFFER = 0, READ_FRAMEBUFFER, DRAW_FRAMEBUFFER };

	struct FramebufferSpec
	{
		uint32_t ColorWidth = 0;
		uint32_t ColorHeight = 0;
		uint32_t DepthWidth = 0;
		uint32_t DepthHeight = 0;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;

		Vec<FrameBufferTextureFormat> ColorAttachment = {};
		FrameBufferTextureFormat DepthAttachment = FrameBufferTextureFormat::NONE;
	};

	Interface FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind(FrameBufferBindMode mode) = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index) const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual const FramebufferSpec& GetSpecification() const = 0;
	};
}