#include "Scene/Component/SceneCamera.h"

using namespace Rocket;

SceneCamera::SceneCamera()
{
	RecalculateProjection();
}

void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
{
	projection_type_ = ProjectionType::Perspective;
	perspective_fov_ = verticalFOV;
	perspective_near_ = nearClip;
	perspective_far_ = farClip;
	RecalculateProjection();
}

void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
{
	projection_type_ = ProjectionType::Orthographic;
	orthographic_size_ = size;
	orthographic_near_ = nearClip;
	orthographic_far_ = farClip;
	RecalculateProjection();
}

void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
{
	aspect_ratio_ = (float)width / (float)height;
	RecalculateProjection();
}

void SceneCamera::RecalculateProjection()
{
	if (projection_type_ == ProjectionType::Perspective)
	{
		projection_ = Matrix4f::Zero();

		const float range = tanf((perspective_fov_ * 0.5f / 180.0f * M_PI)) * orthographic_near_;	
		const float left = -range * aspect_ratio_;
		const float right = range * aspect_ratio_;
		const float bottom = -range;
		const float top = range;

		const float zNear = perspective_near_;
		const float zFar = perspective_far_;
		const float zRange = (zFar - zNear);
        const float tanHalfFOV = tanf((perspective_fov_ / 2.0f / 180.0f * M_PI));
        
		if(0)
		{
			projection_(0,0) = 1.0f / (tanHalfFOV * aspect_ratio_); 
			projection_(1,1) = 1.0f / tanHalfFOV;   
			projection_(2,2) = -(zNear + zFar) / zRange;  
			projection_(2,3) = -2.0f * zFar * zNear / zRange;
			projection_(3,2) = -1.0f;
		}
		else
		{
			projection_(0,0) = (2.0f * zNear) / (right - left);
			projection_(1,1) = (2.0f * zNear) / (top - bottom);
			projection_(2,2) = - (zFar + zNear) / (zRange);
			projection_(2,3) = - 1.0f;
			projection_(3,2) = - (2.0f * zFar * zNear) / (zRange);
		}
	}
	else
	{
		float orthoLeft = -orthographic_size_ * aspect_ratio_ * 0.5f;
		float orthoRight = orthographic_size_ * aspect_ratio_ * 0.5f;
		float orthoBottom = -orthographic_size_ * 0.5f;
		float orthoTop = orthographic_size_ * 0.5f;

		projection_ = Matrix4f::Identity();

		projection_(0,0) = 2.0f / (orthoRight - orthoLeft);
		projection_(1,1) = 2.0f / (orthoTop - orthoBottom);
		projection_(2,2) = - 2.0f / (orthographic_far_ - orthographic_near_);
		projection_(3,0) = - (orthoRight + orthoLeft) / (orthoRight - orthoLeft);
		projection_(3,1) = - (orthoTop + orthoBottom) / (orthoTop - orthoBottom);
		projection_(3,2) = - (orthographic_far_ + orthographic_near_) / (orthographic_far_ - orthographic_near_);
	}
}
