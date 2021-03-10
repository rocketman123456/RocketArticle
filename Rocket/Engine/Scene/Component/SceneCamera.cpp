#include "Scene/Component/SceneCamera.h"

using namespace Rocket;

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
		m_Projection = Matrix4f::Zero();

		const float range = tanf((m_PerspectiveFOV * 0.5f / 180.0f * M_PI)) * m_OrthographicNear;	
		const float left = -range * m_AspectRatio;
		const float right = range * m_AspectRatio;
		const float bottom = -range;
		const float top = range;

		const float zNear = m_PerspectiveNear;
		const float zFar = m_PerspectiveFar;
		const float zRange = (zFar - zNear);
        const float tanHalfFOV = tanf((m_PerspectiveFOV / 2.0f / 180.0f * M_PI));
        
		if(0)
		{
			m_Projection(0,0) = 1.0f / (tanHalfFOV * m_AspectRatio); 
			m_Projection(1,1) = 1.0f / tanHalfFOV;   
			m_Projection(2,2) = -(zNear + zFar) / zRange;  
			m_Projection(2,3) = -2.0f * zFar * zNear / zRange;
			m_Projection(3,2) = -1.0f;
		}
		else
		{
			m_Projection(0,0) = (2.0f * zNear) / (right - left);
			m_Projection(1,1) = (2.0f * zNear) / (top - bottom);
			m_Projection(2,2) = - (zFar + zNear) / (zRange);
			m_Projection(2,3) = - 1.0f;
			m_Projection(3,2) = - (2.0f * zFar * zNear) / (zRange);
		}
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
