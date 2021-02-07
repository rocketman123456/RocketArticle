#pragma once
#include "Scene/SceneComponent.h"
#include "Common/GeomMath.h"

namespace Rocket
{
	class SceneNode;

	class Transform : implements SceneComponent
	{
	public:
		COMPONENT(Transform);
	public:
		Transform(const Transform&) = default;
		Transform(SceneNode& node) : m_Node(node) {}
		Transform(const Matrix4f& mat, SceneNode& node) : m_WorldTransform(mat), m_Node(node) {}
		virtual ~Transform() = default;

		SceneNode& GetNode();

		Vector3f& GetTranslation() { return m_Translation; }
		Quaternionf& GetOrientation() { return m_Orientation; }
		Vector3f& GetScale() { return m_Translation; }
		Matrix4f& GetTransform() { return m_WorldTransform; }
		void SetTranslation(const Vector3f& vec) { m_Translation = vec; Invalidate(); }
		void SetOrientation(const Quaternionf& rot) { m_Orientation = rot; Invalidate();}
		void SetScale(const Vector3f& vec) { m_Translation = vec; Invalidate();}
		void SetTransform(const Matrix4f& mat);

		Matrix4f GetWorldMatrix();

		void Invalidate() { UpdateMatrix = false; }
	private:
		void UpdateTransform();		
	private:
		SceneNode& m_Node;
		bool UpdateMatrix = false;
		Vector3f m_Translation = { 0.0f, 0.0f, 0.0f };
		Vector3f m_Scale = { 1.0f, 1.0f, 1.0f };
		Quaternionf m_Orientation = Quaternionf::Identity();
		Matrix4f m_WorldTransform = Matrix4f::Identity();
	};
}