#pragma once
#include "Core/Core.h"

#include <json.hpp>

namespace Rocket
{
	class GraphNode
	{
	public:
		GraphNode() {};
		GraphNode(size_t id, const String& title, const String& style = NULL, const nlohmann::json& data = {})
		{
			attributes["id"] = id;
			attributes["label"] = title;
			attributes["data"] = data;
			attributes["style"] = style;
		}

		template <typename T>
		static std::uintptr_t handle_to_uintptr_t(T handle)
		{
			return reinterpret_cast<std::uintptr_t>(reinterpret_cast<void*>(handle));
		}

		nlohmann::json attributes;
	};
}
