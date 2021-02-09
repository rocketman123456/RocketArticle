#pragma once
#include "Core/Core.h"

namespace Rocket
{
	ENUM(FramebufferTextureFormat) { NONE = 0, RGB8, RGBA8, RGBA16, RGB16F, RGBA16F, DEPTH24, DEPTH24STENCIL8 };

	struct FramebufferSpecification
	{
		uint32_t ColorWidth = 0;
		uint32_t ColorHeight = 0;
		uint32_t DepthWidth = 0;
		uint32_t DepthHeight = 0;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;

		Vec<FramebufferTextureFormat> ColorAttachment = {};
		FramebufferTextureFormat DepthAttachment = FramebufferTextureFormat::NONE;
	};

	Interface FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index) const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
	};
}