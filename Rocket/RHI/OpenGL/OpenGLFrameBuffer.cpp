#include "OpenGL/OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Rocket
{
	static const uint32_t s_MaxFramebufferSize = 8192;

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpecification& spec) : m_Specification(spec)
	{
		Invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteTextures(1, &m_DepthAttachment);
		glDeleteRenderbuffers(1, &m_DepthStencilAttachment);
	}

	void OpenGLFrameBuffer::Invalidate()
	{
		if (m_RendererID)
		{
			switch (m_Specification.Format)
			{
			case FrameFormat::FRAMEBUFFER: [[fallthrough]];
			case FrameFormat::READ_FRAMEBUFFER: [[fallthrough]];
			case FrameFormat::DRAW_FRAMEBUFFER: [[fallthrough]];
			case FrameFormat::RENDERBUFFER: 
			{
				glDeleteFramebuffers(1, &m_RendererID);
				glDeleteTextures(1, &m_ColorAttachment);
				glDeleteTextures(1, &m_DepthAttachment);
				glDeleteRenderbuffers(1, &m_DepthStencilAttachment);
			} break;
			}
		}
		// Gen Frame Buffer
		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		for (auto Attach : m_Specification.Attachment)
		{
			switch (Attach)
			{
			case FrameAttachment::COLOR_ATTACHMENT:
			{
				// Gen Color Buffer
				glGenTextures(1, &m_ColorAttachment);
				glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
			} break;
			case FrameAttachment::DEPTH_ATTACHMENT:
			{
				// Gen Depth Buffer
				glGenTextures(1, &m_DepthAttachment);
				glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Specification.Width, m_Specification.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);	
			} break;
			case FrameAttachment::DEPTH_STENCIL_ATTACHMENT:
			{
				// Gen Depth Stencil Buffer
				glGenRenderbuffers(1, &m_DepthStencilAttachment);
				glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilAttachment);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilAttachment, 0);
			} break;
			}
		}
		
		RK_GRAPHICS_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Bind()
	{
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLFrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			RK_GRAPHICS_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}
}