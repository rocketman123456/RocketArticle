#pragma once
#include "Render/DrawBasic/BufferLayout.h"

namespace Rocket
{
	Interface IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;
	};
}