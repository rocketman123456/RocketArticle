#include "OpenGL/OpenGLFrameBuffer.h"

#include <glad/glad.h>

using namespace Rocket;

static const uint32_t s_MaxFramebufferSize = 8192;

OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpec& spec) : m_Specification(spec)
{
	Invalidate();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
	if(m_ColorAttachments.size() > 0)
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
	if(m_DepthAttachment)
		glDeleteTextures(1, &m_DepthAttachment);
	glDeleteFramebuffers(1, &m_RendererID);
}

void OpenGLFrameBuffer::Invalidate()
{
	if (m_RendererID)
	{
		if(m_ColorAttachments.size() > 0)
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		if(m_DepthAttachment)
			glDeleteTextures(1, &m_DepthAttachment);
		glDeleteFramebuffers(1, &m_RendererID);
		m_ColorAttachments.clear();
		m_ColorSpecifications.clear();
		m_DepthAttachment = 0;
		m_RendererID = 0;
	}

	// Gen Frame Buffer
	glGenFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	
	// Bind Color Texture
	if (m_Specification.ColorAttachment.size())
	{
		auto sz = m_Specification.ColorAttachment.size();
		m_ColorAttachments.resize(sz);
		m_ColorSpecifications.resize(sz);

		glGenTextures(sz, m_ColorAttachments.data());
		//glActiveTexture(GL_TEXTURE0);

		for (size_t i = 0; i < m_ColorAttachments.size(); i++)
		{
			if(m_Specification.Samples > 1)
			{
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachments[i]);
				switch (m_Specification.ColorAttachment[i])
				{
				case FRAME_TEXTURE_FORMAT::RGB8:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGB8, m_Specification.ColorWidth, m_Specification.ColorHeight, GL_TRUE);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA8:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGBA8, m_Specification.ColorWidth, m_Specification.ColorHeight, GL_TRUE);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA16:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGBA16, m_Specification.ColorWidth, m_Specification.ColorHeight, GL_TRUE);
					break;
				case FRAME_TEXTURE_FORMAT::RGB16F:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGB16F, m_Specification.ColorWidth, m_Specification.ColorHeight, GL_TRUE);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA16F:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGBA16F, m_Specification.ColorWidth, m_Specification.ColorHeight, GL_TRUE);
					break;
				default:
					RK_GRAPHICS_ERROR("Frame Buffer Color Attachment Format Error");
					break;
				}
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachments[i], 0);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);
				switch (m_Specification.ColorAttachment[i])
				{
				case FRAME_TEXTURE_FORMAT::RGB8:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Specification.ColorWidth, m_Specification.ColorHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA8:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.ColorWidth, m_Specification.ColorHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA16:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_Specification.ColorWidth, m_Specification.ColorHeight, 0, GL_RGBA, GL_UNSIGNED_INT, NULL);
					break;
				case FRAME_TEXTURE_FORMAT::RGB16F:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Specification.ColorWidth, m_Specification.ColorHeight, 0, GL_RGB, GL_FLOAT, NULL);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA16F:
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

				glBindTexture(GL_TEXTURE_2D, 0);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i], 0);
			}
			m_ColorSpecifications.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		if (m_ColorSpecifications.size() > 0)
		{
			glDrawBuffers(m_ColorSpecifications.size(), m_ColorSpecifications.data());
		}
		else
		{
			glDrawBuffer(GL_NONE);
		}
	}

	// Bind Depth Texture
	if(m_Specification.DepthAttachment != FRAME_TEXTURE_FORMAT::NONE)
	{
		glGenTextures(1, &m_DepthAttachment);
		//glActiveTexture(GL_TEXTURE0);

		if (m_Specification.Samples > 1)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment);
			switch (m_Specification.DepthAttachment)
			{
			case FRAME_TEXTURE_FORMAT::DEPTH24:
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGB8, m_Specification.DepthWidth, m_Specification.DepthHeight, GL_TRUE);
				break;
			case FRAME_TEXTURE_FORMAT::DEPTH24STENCIL8:
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGBA8, m_Specification.DepthWidth, m_Specification.DepthHeight, GL_TRUE);
				break;
			default:
				RK_GRAPHICS_ERROR("Frame Buffer Color Attachment Format Error");
				break;
			}

			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

			switch (m_Specification.DepthAttachment)
			{
			case FRAME_TEXTURE_FORMAT::DEPTH24:
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment, 0);
				break;
			case FRAME_TEXTURE_FORMAT::DEPTH24STENCIL8:
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment, 0);
				break;
			default:
				RK_GRAPHICS_ERROR("Frame Buffer Color Attachment Format Error");
				break;
			}
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
			switch (m_Specification.DepthAttachment)
			{
			case FRAME_TEXTURE_FORMAT::DEPTH24:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Specification.ColorWidth, m_Specification.ColorHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
				break;
			case FRAME_TEXTURE_FORMAT::DEPTH24STENCIL8:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.ColorWidth, m_Specification.ColorHeight, 0, GL_DEPTH_STENCIL_ATTACHMENT, GL_UNSIGNED_BYTE, NULL);
				break;
			default:
				RK_GRAPHICS_ERROR("Frame Buffer Color Attachment Format Error");
				break;
			}

			//glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glBindTexture(GL_TEXTURE_2D, 0);

			switch (m_Specification.DepthAttachment)
			{
			case FRAME_TEXTURE_FORMAT::DEPTH24:
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0); 
				break;
			case FRAME_TEXTURE_FORMAT::DEPTH24STENCIL8:
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
				break;
			default:
				RK_GRAPHICS_ERROR("Frame Buffer Color Attachment Format Error");
				break;
			}
		}
	}

	RK_GRAPHICS_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Bind(FRAME_BIND_MODE mode)
{
	switch (mode)
	{
	case FRAME_BIND_MODE::FRAMEBUFFER:
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		break;
	case FRAME_BIND_MODE::DRAW_FRAMEBUFFER:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID);
		break;
	case FRAME_BIND_MODE::READ_FRAMEBUFFER:
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
		break;
	}

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
