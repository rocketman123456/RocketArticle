#pragma once
#include "Core/Core.h"
#include "Event/Event.h"

#include <imgui.h>

namespace Rocket
{
	Interface UI
	{
	public:
		virtual void Initialize();
		virtual void Finalize();
		virtual void Draw() = 0;

		void DrawUI();
		virtual void UpdataOverlay(uint32_t width, uint32_t height);
		virtual bool OnUIResponse(EventPtr& e);

	protected:
		// UI Control Variable
		ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		double angle_x;
    	double angle_y;
    	int32_t direction;
		bool showRobotState = true;
	};
}