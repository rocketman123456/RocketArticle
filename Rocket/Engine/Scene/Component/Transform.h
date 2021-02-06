#pragma once
#include "Scene/SceneComponent.h"
#include "Common/GeomMath.h"

namespace Rocket
{
	class Transform : implements SceneComponent
	{
	public:
		COMPONENT(Transform);
	public:
		Transform() = default;
		Transform(const Transform&) = default;
		Transform(const Matrix4f& mat) : TransformMat(mat) {}

		Vector3f& GetPosition() { return Position; }
		Quaternionf& GetRotation() { return Rotation; }
		Vector3f& GetScale() { return Position; }
		Matrix4f& GetTransform() { return TransformMat; }

		void SetPosition(const Vector3f& vec) { Position = vec; Invalidate(); }
		void SetRotation(const Quaternionf& rot) { Rotation = rot; Invalidate();}
		void SetScale(const Vector3f& vec) { Position = vec; Invalidate();}
		void SetTransform(const Matrix4f& mat) { TransformMat = mat; Invalidate();}

		void Invalidate() { UpdateMatrix = false; }
		
	private:
		bool UpdateMatrix = false;
		Vector3f Position = Vector3f::Zero();
		Vector3f Scale = Vector3f::Zero();
		Quaternionf Rotation = Quaternionf(Matrix3f::Identity());
		Matrix4f TransformMat = Matrix4f::Zero();
	};
}