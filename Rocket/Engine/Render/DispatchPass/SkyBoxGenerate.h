#pragma once
#include "Core/Core.h"
#include "Render/DispatchPass/BaseDispatchPass.h"
namespace Rocket
{
	class SkyBoxGenerate : implements BaseDispatchPass
	{
	public:
		void Dispatch(Frame& frame);
	};
}