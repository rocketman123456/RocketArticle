#pragma once
#include "Core/Core.h"
#include "Utils/Timestep.h"
#include "Scene/EditorCamera.h"
#include "Scene/SceneCamera.h"

#include <entt/entt.hpp>

namespace Rocket
{
    class Entity;
    enum class SceneState { Play = 0, Editor };
    class Scene
    {
    public:
        Scene(const std::string& name = "Untitled") : m_Name(name) {}
        ~Scene() = default;

        Entity CreateEntity(const std::string &name = "");
		void DestroyEntity(Entity entity);
		void DestroyAllEntities();

        void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

        Entity GetPrimaryCameraEntity();

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
        entt::registry& GetRegistry() { return m_Registry; }
        uint32_t GetViewWidth() const { return m_ViewportWidth; }
        uint32_t GetViewHeight() const { return m_ViewportHeight; }
        const std::string& GetName() const { return m_Name; }

        template<typename T>
        auto GetComponentView() { return m_Registry.view<T>(); }
        template<typename T1, typename T2>
        auto GetComponentView() { return m_Registry.view<T1, T2>(); }

	private:
        std::string m_Name;
		uint32_t m_ViewportWidth = 0;
        uint32_t m_ViewportHeight = 0;
		entt::registry m_Registry;


        Camera* m_EditorCamera = nullptr;
        Camera* m_PrimaryCamera = nullptr;

        //friend class Entity;
        friend class SceneSerializer;
    };
}