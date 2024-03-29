#pragma once
#include "Core/Core.h"
#include "Utils/Timestep.h"
#include "Scene/SceneComponent.h"
#include "Scene/SceneNode.h"

#include "Scene/Component/SceneCamera.h"
#include "Scene/Component/EditorCamera.h"
#include "Scene/Component/PlanarMesh.h"

namespace Rocket
{
    ENUM(SceneState) { Play = 0, Editor };

    class Scene
    {
    public:
        Scene() = default;
        Scene(const String& name) : m_Name(name) {}
        ~Scene() = default;

        void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		// TODO : Remove For Development
		void SetRenderTarget();
		void GetRenderTarget();

		void SetNodes(Vec<Scope<SceneNode>>&& nodes);
		void AddNode(Scope<SceneNode>&& node);
		void AddChild(SceneNode& child);
		void AddComponent(Scope<SceneComponent>&& component);
		void AddComponent(Scope<SceneComponent>&& component, SceneNode& node);

		SceneNode* FindNode(const String& name);
		void SetRootNode(SceneNode& node);
		SceneNode& GetRootNode();

		template <class T>
		void SetComponents(Vec<Scope<T>>&& components)
		{
			Vec<Scope<SceneComponent>> result(components.size());
			std::transform(components.begin(), components.end(), result.begin(),
				[](Scope<T>& component) -> Scope<SceneComponent> {
					return Scope<SceneComponent>(std::move(component));
				});
			SetComponents(typeid(T), std::move(result));
		}
		void SetComponents(const std::type_index& type_info, Vec<Scope<SceneComponent>>&& components);

		template <class T>
		void ClearComponents()
		{
			SetComponents(typeid(T), {});
		}

		template <class T>
		Vec<T*> GetComponents() const
		{
			Vec<T*> result;
			if (HasComponent(typeid(T)))
			{
				auto& scene_components = GetComponents(typeid(T));

				result.resize(scene_components.size());
				std::transform(scene_components.begin(), scene_components.end(), result.begin(),
					[](const Scope<SceneComponent>& component) -> T* {
						return dynamic_cast<T*>(component.get());
					});
			}

			return result;
		}
		const Vec<Scope<SceneComponent>>& GetComponents(const std::type_index& type_info) const;

		template <class T>
		bool HasComponent() const
		{
			return HasComponent(typeid(T));
		}
		bool HasComponent(const std::type_index& type_info) const;

		inline void SetName(const String& new_name) { m_Name = new_name; }
		inline const String& GetName() const { return m_Name; }
		inline uint32_t GetViewWidth() const { return m_ViewportWidth; }
		inline uint32_t GetViewHeight() const { return m_ViewportHeight; }
		inline bool GetSceneChange() const { return m_SceneChange; }
		inline void SetSceneChange(bool change) { m_SceneChange = change; }

		SceneState GetSceneState() { return m_State; }

		// For Debug
		void SetPrimaryCamera(const Ref<SceneCamera>& camera) { m_PrimaryCamera = camera; }
		void SetEditorCamera(const Ref<EditorCamera>& camera) { m_EditorCamera = camera; }
		Ref<SceneCamera>& GetPrimaryCamera() { return m_PrimaryCamera; }
		Ref<EditorCamera>& GetEditorCamera() { return m_EditorCamera; }
		void SetPrimaryCameraTransform(const Matrix4f& mat) { m_PrimaryCameraTransform = mat; }
		void SetEditorCameraTransform(const Matrix4f& mat) { m_EditorCameraTransform = mat; }
		Matrix4f& GetPrimaryCameraTransform() { return m_PrimaryCameraTransform; }
		Matrix4f& GetEditorCameraTransform() { return m_EditorCameraTransform; }
	private:
        String m_Name;
		uint32_t m_ViewportWidth = 0;
        uint32_t m_ViewportHeight = 0;
        SceneState m_State = SceneState::Play;
        bool m_SceneChange = false;

		Matrix4f m_PrimaryCameraTransform = Matrix4f::Identity();
		Matrix4f m_EditorCameraTransform = Matrix4f::Identity();
		Ref<SceneCamera> m_PrimaryCamera = nullptr;
		Ref<EditorCamera> m_EditorCamera = nullptr;

        SceneNode* m_Root = nullptr;
        Vec<Scope<SceneNode>> m_Nodes;
        UMap<std::type_index, Vec<Scope<SceneComponent>>> m_Components;

        friend class SceneSerializer;
    };
}