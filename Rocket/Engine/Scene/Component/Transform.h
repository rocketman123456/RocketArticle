#pragma once
#include "Scene/SceneComponent.h"
#include "Math/GeomMath.h"

namespace Rocket
{
	class SceneNode;

	class Transform : implements SceneComponent
	{
	public:
		COMPONENT(Transform);
	public:
		Transform(SceneNode& node) : node_(node) {}
		Transform(const Matrix4f& mat, SceneNode& node) : world_transform_(mat), node_(node) {}
		virtual ~Transform() = default;

		SceneNode& GetNode();

		Vector3f& GetTranslation() { return translation_; }
		Quaternionf& GetOrientation() { return orientation_; }
		Vector3f& GetScale() { return translation_; }
		Matrix4f& GetTransform() { return world_transform_; }
		void SetTranslation(const Vector3f& vec) { translation_ = vec; Invalidate(); }
		void SetOrientation(const Quaternionf& rot) { orientation_ = rot; Invalidate();}
		void SetScale(const Vector3f& vec) { translation_ = vec; Invalidate();}
		void SetTransform(const Matrix4f& mat);

		Matrix4f GetWorldMatrix();

		void Invalidate() { update_matrix_ = false; }
	private:
		void UpdateTransform();
	private:
		// TODO : fix eigen3 matrix aligement issue in memory manager
		Matrix4f world_transform_ = Matrix4f::Identity();
		Quaternionf orientation_ = Quaternionf::Identity();
		Vector3f translation_ = Vector3f({ 0.0f, 0.0f, 0.0f });
		Vector3f scale_ = Vector3f({ 1.0f, 1.0f, 1.0f });
		SceneNode& node_;
		bool update_matrix_ = false;
	};
}