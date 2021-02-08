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

        const Matrix4f& GetViewMatrix() const { return m_ViewMatrix; }
		Matrix4f GetViewProjection() const { return m_Projection * m_ViewMatrix; }

        void UpdateView();

    private:
        float m_FOV = 45.0f;
        float m_AspectRatio = 1.778f;
        float m_NearClip = 0.1f;
        float m_FarClip = 1000.0f;

		Matrix4f m_ViewMatrix;
		Vector3f m_Position = Vector3f::Zero();
		Vector3f m_FocalPoint = Vector3f::Zero();
		Vector2f m_InitialMousePosition = Vector2f::Zero();

		float m_Distance = 10.0f;
		float m_Pitch = 0.0f;
        float m_Yaw = 0.0f;

		float m_ViewportWidth = 1280;
        float m_ViewportHeight = 720;
    };
}