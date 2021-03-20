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

		inline uint64_t GetId() const { return id_; }
		inline Transform& GetTransform() { return transform_; }
		inline String& GetTagName() { return tag_.tag_str; }
	protected:
		uint64_t id_;

		TagName tag_;
		Transform transform_;

		SceneNode* parent_{ nullptr };
		Vec<SceneNode*> children_;
		UMap<std::type_index, SceneComponent*> components_;
	};
}