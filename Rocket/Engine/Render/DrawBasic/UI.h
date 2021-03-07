#pragma once
#include "Core/Core.h"
#include "Event/Event.h"

#include <imgui.h>

namespace Rocket
{
	Interface UI
	{
	public:
		virtual void Initialize() = 0;
		virtual void Finalize() = 0;
		virtual void Draw() = 0;

		virtual void UpdataOverlay(uint32_t width, uint32_t height);
		virtual bool OnUIResponse(EventPtr& e);

	protected:
		// UI Control Variable
		ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	};
}