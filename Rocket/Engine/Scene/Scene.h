#pragma once
#include "Core/Core.h"
#include "Utils/Timestep.h"
#include "Scene/EditorCamera.h"
#include "Scene/SceneCamera.h"
#include "Scene/SceneComponent.h"

#include <entt/entt.hpp>

namespace Rocket
{
    class SceneNode;
    ENUM(SceneState) { Play = 0, Editor };

    class Scene
    {
    public:
        Scene() = default;
        Scene(const String& name = "Untitled") : m_Name(name) {}
        ~Scene() = default;

        void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

        SceneNode CreateNode(const String& name = String());
        void DestroyNode(SceneNode node);

        inline uint32_t GetViewWidth() const { return m_ViewportWidth; }
        inline uint32_t GetViewHeight() const { return m_ViewportHeight; }
        inline const String& GetName() const { return m_Name; }
        inline bool GetSceneChange() const { return m_SceneChange; }
        inline void SetSceneChange(bool c) { m_SceneChange = c; }

        SceneState GetSceneState() { return m_State; }
        void SetPrimaryCamera(Ref<Camera> camera) { m_PrimaryCamera = camera; }
        void SetEditorCamera(Ref<Camera> camera) { m_EditorCamera = camera; }
        Ref<Camera> GetPrimaryCamera() { return m_PrimaryCamera; }
        Ref<Camera> GetEditorCamera() { return m_EditorCamera; }
        Matrix4f& GetPrimaryCameraTransform() { return m_PrimaryCameraTransform; }
        Matrix4f& GetEditorCameraTransform() { return m_EditorCameraTransform; }

	private:
        String m_Name;
		uint32_t m_ViewportWidth = 0;
        uint32_t m_ViewportHeight = 0;
        bool m_SceneChange = false;
        SceneState m_State = SceneState::Play;

        Ref<Camera> m_PrimaryCamera = nullptr;
        Ref<Camera> m_EditorCamera = nullptr;
        Matrix4f m_PrimaryCameraTransform = Matrix4f::Identity();
        Matrix4f m_EditorCameraTransform = Matrix4f::Identity();

        entt::registry m_Registry;

        friend class SceneNode;
        friend class SceneSerializer;
    };
}