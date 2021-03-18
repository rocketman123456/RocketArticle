#pragma once
#include "Scene/SceneComponent.h"
#include "Math/GeomMath.h"

namespace Rocket
{
	struct alignas(16) MeshVertex {
		Vector3f pos;		// 12 bytes
		Vector3f normal;	// 12 bytes
		Vector2f uv0;		// 8 bytes
		Vector2f uv1;		// 8 bytes
		Vector4f joint0;	// 16 bytes
		Vector4f weight0;	// 16 bytes
	};						// total 72 bytes

	struct MeshVertices {
		//VkBuffer buffer = VK_NULL_HANDLE;
		//VkDeviceMemory memory;
	};

	struct MeshIndices {
		int count;
		//VkBuffer buffer = VK_NULL_HANDLE;
		//VkDeviceMemory memory;
	};

	struct BoundingBox {
		Vector3f minPos;	// 12 bytes
		Vector3f maxPos;	// 12 bytes
		bool valid = false;	// 1 bytes
		BoundingBox() {}
		BoundingBox(const Vector3f& min, const Vector3f& max) : minPos(min), maxPos(max) {}
		BoundingBox GetAABB(const Matrix4f& m);
	};

    Interface Mesh : implements SceneComponent
    {
    public:
        virtual ~Mesh() = default;
    };
}