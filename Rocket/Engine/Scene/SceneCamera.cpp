#include "Scene/SceneCamera.h"

namespace Rocket
{
    SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

    void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}

    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

    void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
            float range = tan((m_PerspectiveFOV / 2.0f / 180.0f * M_PI)) * m_OrthographicNear;	
            float left = -range * m_AspectRatio;
            float right = range * m_AspectRatio;
            float bottom = -range;
            float top = range;

			m_Projection = Matrix4f::Zero();
            
            m_Projection(0,0) = (2.0f * m_PerspectiveNear) / (right - left);
            m_Projection(1,1) = (2.0f * m_PerspectiveNear) / (top - bottom);
            m_Projection(2,2) = - (m_PerspectiveFar + m_PerspectiveNear) / (m_PerspectiveFar - m_PerspectiveNear);
            m_Projection(2,3) = - 1.0f;
            m_Projection(3,2) = - (2.0f * m_PerspectiveFar * m_PerspectiveNear) / (m_PerspectiveFar - m_PerspectiveNear);
		}
		else
		{
			float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoBottom = -m_OrthographicSize * 0.5f;
			float orthoTop = m_OrthographicSize * 0.5f;

			m_Projection = Matrix4f::Identity();

            m_Projection(0,0) = 2.0f / (orthoRight - orthoLeft);
            m_Projection(1,1) = 2.0f / (orthoTop - orthoBottom);
            m_Projection(2,2) = - 2.0f / (m_OrthographicFar - m_OrthographicNear);
            m_Projection(3,0) = - (orthoRight + orthoLeft) / (orthoRight - orthoLeft);
            m_Projection(3,1) = - (orthoTop + orthoBottom) / (orthoTop - orthoBottom);
            m_Projection(3,2) = - (m_OrthographicFar + m_OrthographicNear) / (m_OrthographicFar - m_OrthographicNear);
        }
		
	}
}