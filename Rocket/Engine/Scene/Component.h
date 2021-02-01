#pragma once
#include "Core/Core.h"
#include "Common/GeomMath.h"
#include "Scene/SceneCamera.h"
#include "Render/DrawBasic/Texture.h"

#include <string>

namespace Rocket
{
    struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
	};

	struct TransformComponent
	{
		Vector3f Translation = { 0.0f, 0.0f, 0.0f };
		Vector3f Rotation = { 0.0f, 0.0f, 0.0f }; 		// RPY-ZYX
		Vector3f Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const Vector3f& translation) : Translation(translation) {}

		Matrix4f GetTransform() const
		{
			Eigen::AngleAxisf v_z((Rotation[0]/180.0f) * M_PI , Eigen::Vector3f::UnitZ());
    		Eigen::AngleAxisf v_y((Rotation[1]/180.0f) * M_PI , Eigen::Vector3f::UnitY());
    		Eigen::AngleAxisf v_x((Rotation[2]/180.0f) * M_PI , Eigen::Vector3f::UnitX());
			Matrix4f rot_tran = Matrix4f::Identity();
			rot_tran.block<3,3>(0,0) = v_x.matrix() * v_y.matrix() * v_z.matrix();
			rot_tran(0, 3) = Translation[0];
			rot_tran(1, 3) = Translation[1];
			rot_tran(2, 3) = Translation[2];
			Matrix4f mat_s = Matrix4f::Identity();
			mat_s(0, 0) = Scale[0];
			mat_s(1, 1) = Scale[1];
			mat_s(2, 2) = Scale[2];
			return rot_tran * mat_s;
		}
	};

	struct SpriteRendererComponent
	{
		Vector4f Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const Vector4f& color) : Color(color) {}
	};

	struct TextureRendererComponent
	{
		Ref<Texture2D> Texture;

		TextureRendererComponent() = default;
		TextureRendererComponent(const TextureRendererComponent&) = default;
		TextureRendererComponent(const Ref<Texture2D>& texture) : Texture(texture) {}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};
}