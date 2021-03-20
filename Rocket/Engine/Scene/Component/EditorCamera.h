#pragma once
#include "Scene/Component/Camera.h"

namespace Rocket
{
    class EditorCamera : implements Camera
    {
    public:
		COMPONENT(EditorCamera);
    public:
        EditorCamera() = default;
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);
        virtual ~EditorCamera() = default;

        const Matrix4f& GetViewMatrix() const { return view_matrix_; }
		Matrix4f GetViewProjection() const { return projection_ * view_matrix_; }

        void UpdateView();

    private:
        float fov_ = 45.0f;
        float aspect_ratio_ = 1.778f;
        float near_clip_ = 0.1f;
        float far_clip_ = 1000.0f;

		Matrix4f view_matrix_;
		Vector3f position_ = Vector3f::Zero();
		Vector3f focal_point_ = Vector3f::Zero();
		Vector2f initial_mouse_position_ = Vector2f::Zero();

		float distance_ = 10.0f;
		float pitch_ = 0.0f;
        float yaw_ = 0.0f;

		float viewport_width_ = 1280;
        float viewport_height_ = 720;
    };
}