#pragma once
#include "Core/Core.h"

namespace Rocket
{
	Interface UI
	{
	public:
		virtual void Initialize() = 0;
		virtual void Finalize() = 0;

		virtual void UpdataOverlay(uint32_t width, uint32_t height) = 0;
		virtual void Draw() = 0;
	};
}