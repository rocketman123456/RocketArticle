#pragma once
#include "Core/Core.h"
#include "Scene/Scene.h"

#include <entt/entt.hpp>
#include <utility>

namespace Rocket
{
    class Entity
    {
    public:
        Entity() = default;
		Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}
		Entity(const Entity& other) = default;

        template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			RK_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            T& component = m_Scene->GetRegistry()->emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
            m_Scene->OnComponentAdded<T>(*this, component);
			m_Scene->SetSceneChange(true);
            return component;
		}

        template<typename T>
		T& GetComponent()
		{
			RK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->GetRegistry()->get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
            return m_Scene->GetRegistry()->has<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			RK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->GetRegistry()->remove<T>(m_EntityHandle);
			m_Scene->SetSceneChange(true);
		}

        operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
        bool operator==(const Entity& o) const { return m_EntityHandle == o.m_EntityHandle && m_Scene == o.m_Scene; }
		bool operator!=(const Entity& o) const { return !(*this == o); }

    private:
        entt::entity m_EntityHandle{ entt::null };
        Scene* m_Scene;
    };
}