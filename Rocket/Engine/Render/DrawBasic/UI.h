#pragma once
#include "Core/Core.h"
#include "Event/Event.h"

#include <imgui.h>

namespace Rocket
{
	Interface UIContext
	{
	public:
		virtual void Draw() = 0;
	};

	Interface UI
	{
	public:
		virtual void Initialize();
		virtual void Finalize();
		virtual void Draw() = 0;

		void AddContext(const Ref<UIContext>& context) { contexts.push_back(context); }
		// TODO : add remove ui function
		void DrawUI();
		virtual void UpdataOverlay(uint32_t width, uint32_t height);
		virtual bool OnUIResponse(EventPtr& e);

	protected:
		// TODO : use map to store
		Vec<Ref<UIContext>> contexts;
	};
}