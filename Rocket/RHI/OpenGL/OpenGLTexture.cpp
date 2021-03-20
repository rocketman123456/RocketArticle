#include "OpenGL/OpenGLTexture.h"
#include "Module/AssetLoader.h"

#include <stb_image.h>

using namespace Rocket;

OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
	: width_(width), height_(height)
{
	internal_format_ = GL_RGBA8;
	data_format_ = GL_RGBA;

	glGenTextures(1, &renderer_id_);
	glBindTexture(GL_TEXTURE_2D, renderer_id_);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

OpenGLTexture2D::OpenGLTexture2D(const String& path)
	: path_(path)
{
	int channels;
	auto data = g_AssetLoader->SyncOpenAndReadTexture(path, &width_, &height_, &channels);

	GLenum internalFormat = 0, dataFormat = 0;
	if (channels == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (channels == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	internal_format_ = internalFormat;
	data_format_ = dataFormat;

	RK_GRAPHICS_ASSERT(internalFormat & dataFormat, "Format not supported!");

	glGenTextures(1, &renderer_id_);
	glBindTexture(GL_TEXTURE_2D, renderer_id_);

	glGenerateMipmap(GL_TEXTURE_2D);

	// set the texture wrapping parameters
	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, data_format_, width_, height_, 0, data_format_, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	g_AssetLoader->SyncCloseTexture(data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &renderer_id_);
}

void OpenGLTexture2D::SetData(void* data, uint32_t size)
{
	uint32_t bpp = data_format_ == GL_RGBA ? 4 : 3;
	RK_GRAPHICS_ASSERT(size == width_ * height_ * bpp, "Data must be entire texture!");
	glTexImage2D(GL_TEXTURE_2D, 0, data_format_, width_, height_, 0, data_format_, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, renderer_id_);
}
void OpenGLTexture2D::Unbind(uint32_t slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, 0);
}
