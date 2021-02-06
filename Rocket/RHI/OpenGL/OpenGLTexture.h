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

		uint32_t GetWidth() const final { return m_Width; }
		uint32_t GetHeight() const final { return m_Height; }
		uint32_t GetRendererID() const final { return m_RendererID; }

		void SetData(void* data, uint32_t size) final;

		void Bind(uint32_t slot = 0) const final;
		void Unbind(uint32_t slot = 0) const final;

		bool operator==(const Texture& other) const final
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

	private:
		String m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}