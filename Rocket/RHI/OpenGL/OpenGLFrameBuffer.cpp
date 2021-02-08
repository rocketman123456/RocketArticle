#include "OpenGL/OpenGLFrameBuffer.h"

#include <glad/glad.h>

using namespace Rocket;

static const uint32_t s_MaxFramebufferSize = 8192;

OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpecification& spec) : m_Specification(spec)
{
	Invalidate();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
	glDeleteFramebuffers(1, &m_RendererID);
	if(m_ColorAttachments.size() > 0)
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
	if(m_DepthAttachment)
		glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGLFrameBuffer::Invalidate()
{
	if (m_RendererID)
	{
		glDeleteFramebuffers(1, &m_RendererID);
		if(m_ColorAttachments.size() > 0)
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		if(m_DepthAttachment)
			glDeleteTextures(1, &m_DepthAttachment);
		m_ColorAttachments.clear();
		m_DepthAttachment = 0;
	}

	// Gen Frame Buffer
	glGenFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	
	if (m_Specification.ColorAttachment.size())
	{
		auto sz = m_Specification.ColorAttachment.size();
		m_ColorAttachments.resize(sz);
		m_ColorSpecifications.resize(sz);

		glGenTextures(sz, m_ColorAttachments.data());

		for (size_t i = 0; i < m_ColorAttachments.size(); i++)
		{
			if(m_Specification.Samples > 1)
			{
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachments[i]);
				switch (m_Specification.ColorAttachment[i])
				{
				case FramebufferTextureFormat::RGB8:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGB8, m_Specification.ColorWidth, m_Specification.ColorHeight, GL_TRUE);
					break;
				case FramebufferTextureFormat::RGBA8:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGBA8, m_Specification.ColorWidth, m_Specification.ColorHeight, GL_TRUE);
					break;
				case FramebufferTextureFormat::RGBA16:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGBA16, m_Specification.ColorWidth, m_Specification.ColorHeight, GL_TRUE);
					break;
				case FramebufferTextureFormat::RGB16F:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGB16F, m_Specification.ColorWidth, m_Specification.ColorHeight, GL_TRUE);
					break;
				case FramebufferTextureFormat::RGBA16F:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGBA16F, m_Specification.ColorWidth, m_Specification.ColorHeight, GL_TRUE);
					break;
				default:
					RK_GRAPHICS_ERROR("Frame Buffer Color Attachment Format Error");
					break;
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachments[i], 0);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);
				switch (m_Specification.ColorAttachment[i])
				{
				case FramebufferTextureFormat::RGB8:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Specification.ColorWidth, m_Specification.ColorHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
					break;
				case FramebufferTextureFormat::RGBA8:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.ColorWidth, m_Specification.ColorHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
					break;
				case FramebufferTextureFormat::RGBA16:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_Specification.ColorWidth, m_Specification.ColorHeight, 0, GL_RGB, GL_UNSIGNED_SHORT, NULL);
					break;
				case FramebufferTextureFormat::RGB16F:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Specification.ColorWidth, m_Specification.ColorHeight, 0, GL_RGB, GL_FLOAT, NULL);
					break;
				case FramebufferTextureFormat::RGBA16F:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Specification.ColorWidth, m_Specification.ColorHeight, 0, GL_RGBA, GL_FLOAT, NULL);
					break;
				default:
					RK_GRAPHICS_ERROR("Frame Buffer Color Attachment Format Error");
					break;
				}
				
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i], 0);
			}
			m_ColorSpecifications.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
	}

	if(m_ColorSpecifications.size() > 1)
	{
		glDrawBuffers(m_ColorSpecifications.size(), m_ColorSpecifications.data());
	}

	if(m_Specification.DepthAttachment != FramebufferTextureFormat::None)
	{
		glGenRenderbuffers(1, &m_DepthAttachment);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
		if(m_Specification.Samples > 1)
		{
			switch(m_Specification.DepthAttachment)
			{
			case FramebufferTextureFormat::DEPTH24:
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Specification.Samples, GL_DEPTH_COMPONENT, m_Specification.DepthWidth, m_Specification.DepthHeight);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);
				break;
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Specification.Samples, GL_DEPTH24_STENCIL8, m_Specification.DepthWidth, m_Specification.DepthHeight);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_RENDERBUFFER, m_DepthAttachment);
				break;
			default:
				RK_GRAPHICS_ERROR("Frame Buffer Depth Stencil Attachment Format Error");
				break;
			}
		}
		else
		{
			switch(m_Specification.DepthAttachment)
			{
			case FramebufferTextureFormat::DEPTH24:
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Specification.DepthWidth, m_Specification.DepthHeight);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);
				break;
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Specification.DepthWidth, m_Specification.DepthHeight);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_RENDERBUFFER, m_DepthAttachment);
				break;
			default:
				RK_GRAPHICS_ERROR("Frame Buffer Depth Stencil Attachment Format Error");
				break;
			}
		}
	}

	RK_GRAPHICS_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glViewport(0, 0, m_Specification.ColorWidth, m_Specification.ColorHeight);
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
	m_Specification.ColorWidth = width;
	m_Specification.ColorHeight = height;
	m_Specification.DepthWidth = width;
	m_Specification.DepthHeight = height;
	Invalidate();
}
