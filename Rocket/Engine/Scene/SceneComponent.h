#pragma once
#include "Scene/SceneComponentDef.h"
#include "Utils/Hashing.h"

#include <typeindex>

namespace Rocket
{
	Interface SceneComponent
	{
	public:
		SceneComponent();
		SceneComponent(SceneComponent&& other) = default;
		virtual ~SceneComponent() = default;

		virtual std::type_index GetType() = 0;
		const uint64_t GetId() const { return m_Id; }
	protected:
		uint64_t m_Id;
	};

	std::ostream& operator<<(std::ostream& out, SceneComponent& com);

	#define COMPONENT(x) std::type_index GetType() final { return typeid(x); }
}