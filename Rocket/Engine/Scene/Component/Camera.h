#pragma once
#include "Math/GeomMath.h"
#include "Scene/SceneComponent.h"

namespace Rocket
{
    Interface Camera : implements SceneComponent
	{
	public:
		Camera() = default;
		Camera(const Matrix4f& projection) : projection_(projection) {}
		virtual ~Camera() = default;

		const Matrix4f& GetProjection() const { return projection_; }
	protected:
		Matrix4f projection_ = Matrix4f::Zero();
	};
}