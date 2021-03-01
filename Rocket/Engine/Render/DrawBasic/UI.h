#pragma once
#include "Core/Core.h"

namespace Rocket
{
	Interface UI
	{
	public:
		virtual void Initialize() = 0;
		virtual void Finalize() = 0;
	};
}