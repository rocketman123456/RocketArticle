#include "OpenGL/OpenGLFrameBuffer.h"

#include <glad/glad.h>

using namespace Rocket;

static const uint32_t s_MaxFramebufferSize = 8192;

OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpec& spec) : specification_(spec)
{
	Invalidate();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
	if(color_attachments_.size() > 0)
		glDeleteTextures(color_attachments_.size(), color_attachments_.data());
	if(depth_attachment_)
		glDeleteTextures(1, &depth_attachment_);
	glDeleteFramebuffers(1, &renderer_id_);
}

void OpenGLFrameBuffer::Invalidate()
{
	if (renderer_id_)
	{
		if(color_attachments_.size() > 0)
			glDeleteTextures(color_attachments_.size(), color_attachments_.data());
		if(depth_attachment_)
			glDeleteTextures(1, &depth_attachment_);
		glDeleteFramebuffers(1, &renderer_id_);
		color_attachments_.clear();
		color_specifications_.clear();
		depth_attachment_ = 0;
		renderer_id_ = 0;
	}

	// Gen Frame Buffer
	glGenFramebuffers(1, &renderer_id_);
	glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);
	
	// Bind Color Texture
	if (specification_.color_attachment.size())
	{
		auto sz = specification_.color_attachment.size();
		color_attachments_.resize(sz);
		color_specifications_.resize(sz);

		glGenTextures(sz, color_attachments_.data());
		//glActiveTexture(GL_TEXTURE0);

		for (size_t i = 0; i < color_attachments_.size(); i++)
		{
			if(specification_.samples > 1)
			{
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_attachments_[i]);
				switch (specification_.color_attachment[i])
				{
				case FRAME_TEXTURE_FORMAT::RGB8:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification_.samples, GL_RGB8, specification_.color_width, specification_.color_height, GL_TRUE);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA8:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification_.samples, GL_RGBA8, specification_.color_width, specification_.color_height, GL_TRUE);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA16:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification_.samples, GL_RGBA16, specification_.color_width, specification_.color_height, GL_TRUE);
					break;
				case FRAME_TEXTURE_FORMAT::RGB16F:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification_.samples, GL_RGB16F, specification_.color_width, specification_.color_height, GL_TRUE);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA16F:
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification_.samples, GL_RGBA16F, specification_.color_width, specification_.color_height, GL_TRUE);
					break;
				default:
					RK_GRAPHICS_ERROR("Frame Buffer Color Attachment Format Error");
					break;
				}
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, color_attachments_[i], 0);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, color_attachments_[i]);
				switch (specification_.color_attachment[i])
				{
				case FRAME_TEXTURE_FORMAT::RGB8:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, specification_.color_width, specification_.color_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA8:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specification_.color_width, specification_.color_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA16:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, specification_.color_width, specification_.color_height, 0, GL_RGBA, GL_UNSIGNED_INT, NULL);
					break;
				case FRAME_TEXTURE_FORMAT::RGB16F:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, specification_.color_width, specification_.color_height, 0, GL_RGB, GL_FLOAT, NULL);
					break;
				case FRAME_TEXTURE_FORMAT::RGBA16F:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, specification_.color_width, specification_.color_height, 0, GL_RGBA, GL_FLOAT, NULL);
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

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color_attachments_[i], 0);
			}
			color_specifications_.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		if (color_specifications_.size() > 0)
		{
			glDrawBuffers(color_specifications_.size(), color_specifications_.data());
		}
		else
		{
			glDrawBuffer(GL_NONE);
		}
	}

	// Bind Depth Texture
	if(specification_.depth_attachment != FRAME_TEXTURE_FORMAT::NONE)
	{
		glGenTextures(1, &depth_attachment_);
		//glActiveTexture(GL_TEXTURE0);

		if (specification_.samples > 1)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_attachment_);
			switch (specification_.depth_attachment)
			{
			case FRAME_TEXTURE_FORMAT::DEPTH24:
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification_.samples, GL_RGB8, specification_.depth_width, specification_.depth_height, GL_TRUE);
				break;
			case FRAME_TEXTURE_FORMAT::DEPTH24STENCIL8:
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification_.samples, GL_RGBA8, specification_.depth_width, specification_.depth_height, GL_TRUE);
				break;
			default:
				RK_GRAPHICS_ERROR("Frame Buffer Color Attachment Format Error");
				break;
			}

			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

			switch (specification_.depth_attachment)
			{
			case FRAME_TEXTURE_FORMAT::DEPTH24:
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depth_attachment_, 0);
				break;
			case FRAME_TEXTURE_FORMAT::DEPTH24STENCIL8:
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depth_attachment_, 0);
				break;
			default:
				RK_GRAPHICS_ERROR("Frame Buffer Color Attachment Format Error");
				break;
			}
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, depth_attachment_);
			switch (specification_.depth_attachment)
			{
			case FRAME_TEXTURE_FORMAT::DEPTH24:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, specification_.color_width, specification_.color_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
				break;
			case FRAME_TEXTURE_FORMAT::DEPTH24STENCIL8:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL_ATTACHMENT, specification_.color_width, specification_.color_height, 0, GL_DEPTH_STENCIL_ATTACHMENT, GL_UNSIGNED_BYTE, NULL);
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

			switch (specification_.depth_attachment)
			{
			case FRAME_TEXTURE_FORMAT::DEPTH24:
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_, 0); 
				break;
			case FRAME_TEXTURE_FORMAT::DEPTH24STENCIL8:
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_, 0);
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
		glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);
		break;
	case FRAME_BIND_MODE::DRAW_FRAMEBUFFER:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer_id_);
		break;
	case FRAME_BIND_MODE::READ_FRAMEBUFFER:
		glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer_id_);
		break;
	}

	glViewport(0, 0, specification_.color_width, specification_.color_height);
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
	specification_.color_width = width;
	specification_.color_height = height;
	specification_.depth_width = width;
	specification_.depth_height = height;
	Invalidate();
}
