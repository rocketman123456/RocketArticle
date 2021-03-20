#pragma once
#include "Scene/Component/Camera.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Rocket
{
    class SceneCamera : implements Camera
    {
	public:
		COMPONENT(SceneCamera);
    public:
		ENUM(ProjectionType) { Perspective = 0, Orthographic = 1 };
	public:
		SceneCamera();
		SceneCamera(const Matrix4f& projection) : Camera(projection) {}
		virtual ~SceneCamera() = default;

		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFOV() const { return perspective_fov_; }
		void SetPerspectiveVerticalFOV(float verticalFov) { perspective_fov_ = verticalFov; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return perspective_near_; }
		void SetPerspectiveNearClip(float nearClip) { perspective_near_ = nearClip; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return perspective_far_; }
		void SetPerspectiveFarClip(float farClip) { perspective_far_ = farClip; RecalculateProjection(); }

		float GetOrthographicSize() const { return orthographic_size_; }
		void SetOrthographicSize(float size) { orthographic_size_ = size; RecalculateProjection(); }
		float GetOrthographicNearClip() const { return orthographic_near_; }
		void SetOrthographicNearClip(float nearClip) { orthographic_near_ = nearClip; RecalculateProjection(); }
		float GetOrthographicFarClip() const { return orthographic_far_; }
		void SetOrthographicFarClip(float farClip) { orthographic_far_ = farClip; RecalculateProjection(); }

		ProjectionType GetProjectionType() const { return projection_type_; }
		void SetProjectionType(ProjectionType type) { projection_type_ = type; RecalculateProjection(); }
	private:
		void RecalculateProjection();
	private:
		ProjectionType projection_type_ = ProjectionType::Orthographic;

		float perspective_fov_ = 45.0f / 180.0f * M_PI;
		float perspective_near_ = 0.01f;
        float perspective_far_ = 1000.0f;

		float orthographic_size_ = 10.0f;
		float orthographic_near_ = -1.0f;
        float orthographic_far_ = 1.0f;

		float aspect_ratio_ = 0.0f;
    };
}