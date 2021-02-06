#pragma once
#include "Core/Core.h"
#include "Scene/Scene.h"
#include "Scene/SceneComponent.h"
#include "Utils/Hashing.h"

#include <typeindex>
#include <entt/entt.hpp>

namespace Rocket
{
	Interface SceneNode
	{
	public:
		SceneNode();
		SceneNode(entt::entity handle, Scene* scene);
		SceneNode(const SceneNode& other) = default;
		~SceneNode() = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			RK_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			RK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			RK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		bool operator==(const SceneNode& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const SceneNode& other) const
		{
			return !(*this == other);
		}

		inline uint64_t GetId() const { return m_Id; }
	protected:
		uint64_t m_Id;
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};
}