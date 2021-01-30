#pragma once
#include "Core/Core.h"

#include <entt/entt.hpp>

namespace Rocket
{
    class Scene
    {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string &name = "");
		void DestroyEntity(Entity entity);
		void DestroyAllEntities();

        void OnUpdate(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

    private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

        friend class Entity;
    };
}