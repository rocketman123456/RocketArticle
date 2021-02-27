#pragma once
#include "Core/Core.h"
#include "Scene/SceneComponent.h"
#include "Scene/Component/TagName.h"
#include "Scene/Component/Transform.h"
#include "Utils/Hashing.h"

#include <typeindex>

namespace Rocket
{
	class SceneNode
	{
	public:
		SceneNode();
		SceneNode(const String& name);
		~SceneNode() = default;

		void SetParent(SceneNode& parent);
		SceneNode* GetParent() const;
		void AddChild(SceneNode& child);
		const Vec<SceneNode*>& GetChildren() const;
		void SetComponent(SceneComponent& component);

		template <class T>
		inline T& GetComponent()
		{
			return dynamic_cast<T&>(GetComponent(typeid(T)));
		}
		SceneComponent& GetComponent(const std::type_index index);

		template <class T>
		bool HasComponent()
		{
			return HasComponent(typeid(T));
		}
		bool HasComponent(const std::type_index index);

		inline uint64_t GetId() const { return m_Id; }
		inline Transform& GetTransform() { return m_Transform; }
		inline String& GetTagName() { return m_Tag.TagStr; }
	protected:
		uint64_t m_Id;

		TagName m_Tag;
		Transform m_Transform;

		SceneNode* m_Parent{ nullptr };
		Vec<SceneNode*> m_Children;
		UMap<std::type_index, SceneComponent*> m_Components;
	};
}