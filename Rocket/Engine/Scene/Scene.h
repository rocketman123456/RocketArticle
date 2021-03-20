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
        Scene(const String& name) : name_(name) {}
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

		inline void SetName(const String& new_name) { name_ = new_name; }
		inline const String& GetName() const { return name_; }
		inline uint32_t GetViewWidth() const { return viewport_width_; }
		inline uint32_t GetViewHeight() const { return viewport_height_; }
		inline bool GetSceneChange() const { return scene_change_; }
		inline void SetSceneChange(bool change) { scene_change_ = change; }

		SceneState GetSceneState() { return state_; }

		// For Debug
		void SetPrimaryCamera(const Ref<SceneCamera>& camera) { primary_camera_ = camera; }
		void SetEditorCamera(const Ref<EditorCamera>& camera) { editor_camera_ = camera; }
		Ref<SceneCamera>& GetPrimaryCamera() { return primary_camera_; }
		Ref<EditorCamera>& GetEditorCamera() { return editor_camera_; }
		void SetPrimaryCameraTransform(const Matrix4f& mat) { primary_camera_transform_ = mat; }
		void SetEditorCameraTransform(const Matrix4f& mat) { editor_camera_transform_ = mat; }
		Matrix4f& GetPrimaryCameraTransform() { return primary_camera_transform_; }
		Matrix4f& GetEditorCameraTransform() { return editor_camera_transform_; }
	private:
        String name_;
		uint32_t viewport_width_ = 0;
        uint32_t viewport_height_ = 0;
        SceneState state_ = SceneState::Play;
        bool scene_change_ = false;

		Matrix4f primary_camera_transform_ = Matrix4f::Identity();
		Matrix4f editor_camera_transform_ = Matrix4f::Identity();
		Ref<SceneCamera> primary_camera_ = nullptr;
		Ref<EditorCamera> editor_camera_ = nullptr;

        SceneNode* root_ = nullptr;
        Vec<Scope<SceneNode>> nodes_;
        UMap<std::type_index, Vec<Scope<SceneComponent>>> components_;

        friend class SceneSerializer;
    };
}