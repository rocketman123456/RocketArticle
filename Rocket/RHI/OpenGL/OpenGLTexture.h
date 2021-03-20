#pragma once
#include "Render/DrawBasic/Texture.h"

#include <glad/glad.h>

namespace Rocket
{
	class OpenGLTexture2D : implements Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const String& path);
		virtual ~OpenGLTexture2D();

		uint32_t GetWidth() const final { return width_; }
		uint32_t GetHeight() const final { return height_; }
		uint32_t GetRendererID() const final { return renderer_id_; }

		void SetData(void* data, uint32_t size) final;

		void Bind(uint32_t slot = 0) const final;
		void Unbind(uint32_t slot = 0) const final;

		bool operator==(const Texture& other) const final
		{
			return renderer_id_ == ((OpenGLTexture2D&)other).renderer_id_;
		}

	private:
		String path_;
		int32_t width_, height_;
		uint32_t renderer_id_;
		GLenum internal_format_, data_format_;
	};
}