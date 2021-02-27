#pragma once
#include "Common/GeomMath.h"
#include "Scene/SceneComponent.h"

namespace Rocket
{
    Interface Camera : implements SceneComponent
	{
	public:
		Camera() = default;
		Camera(const Matrix4f& projection) : m_Projection(projection) {}
		virtual ~Camera() = default;

		const Matrix4f& GetProjection() const { return m_Projection; }
	protected:
		Matrix4f m_Projection = Matrix4f::Zero();
	};
}