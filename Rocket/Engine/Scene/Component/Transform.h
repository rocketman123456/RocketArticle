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
		// TODO : fix eigen3 matrix aligement issue in memory manager
		Matrix4f m_WorldTransform = Matrix4f::Identity();
		Quaternionf m_Orientation = Quaternionf::Identity();
		Vector3f m_Translation = Vector3f({ 0.0f, 0.0f, 0.0f });
		Vector3f m_Scale = Vector3f({ 1.0f, 1.0f, 1.0f });
		SceneNode& m_Node;
		bool UpdateMatrix = false;
	};
}