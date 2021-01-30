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
		Entity(entt::entity handle, const Ref<Scene>& scene) : m_EntityHandle(handle), m_Scene(scene) {}
		Entity(const Entity& other) = default;

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
		bool HasComponent()
		{
            auto scene = m_Scene.lock();
            return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
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