#pragma once
#include "Core/Core.h"
#include "Math/GeomMath.h"

namespace Rocket
{
	Interface Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture & other) const = 0;
	};

	Interface Texture2D : inheritance Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const String& path);
	};

	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const Vector2f& min, const Vector2f& max);
		SubTexture2D(const Ref<Texture2D>& texture, const Vector2f& coord, const Vector2f& cellSize, const Vector2f& spriteSize);

		const Ref<Texture2D> GetTexture() { return texture_; }
		const Vector2f* GetTexCoords() { return tex_coords_; }

	private:
		Ref<Texture2D> texture_;
		Vector2f tex_coords_[4];
	};
}