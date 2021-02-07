#pragma once
#include "Scene/SceneComponent.h"
#include "Common/GeomMath.h"
#include "Render/DrawBasic/Texture.h"

namespace Rocket
{
    struct QuadVertex
	{
		Vector3f Position;  // 12 bytes
		Vector4f Color;     // 16 bytes
		Vector2f TexCoord;  // 8 bytes
		float TexIndex;     // 4 bytes
		float TilingFactor; // 4 bytes
	};                      // total 44 bytes

    struct QuadIndex
    {
        uint32_t index[6];  // 12 bytes
    };

    class PlanarMesh : implements SceneComponent
    {
    public:
        COMPONENT(PlanarMesh);
    public:
        PlanarMesh();
        PlanarMesh(const String& name);
        virtual ~PlanarMesh() = default;

        void AddQuad(const Vector2f& position, const Vector2f& size, const Vector4f& color);
		void AddQuad(const Vector3f& position, const Vector2f& size, const Vector4f& color);
		void AddQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const Vector4f& tintColor = Vector4f::Ones());
		void AddQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const Vector4f& tintColor = Vector4f::Ones());

		void AddQuad(const Matrix4f& transform, const Vector4f& color);
		void AddQuad(const Matrix4f& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const Vector4f& tintColor = Vector4f::Ones());

		void AddRotatedQuad(const Vector2f& position, const Vector2f& size, float rotation, const Vector4f& color);
		void AddRotatedQuad(const Vector3f& position, const Vector2f& size, float rotation, const Vector4f& color);
		void AddRotatedQuad(const Vector2f& position, const Vector2f& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const Vector4f& tintColor = Vector4f::Ones());
		void AddRotatedQuad(const Vector3f& position, const Vector2f& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const Vector4f& tintColor = Vector4f::Ones());

        Vec<QuadVertex>& GetVertex() { return m_Vertex; }
        Vec<QuadIndex>& GetIndex() { return m_Index; }
        Vec<Ref<Texture2D>>& GetTexture() { return m_TextureSlots; }
    private:
        String m_Name;
        Vector4f m_QuadVertexPositions[4];
        Vec<QuadVertex> m_Vertex;
        Vec<QuadIndex> m_Index;
        Vec<Ref<Texture2D>> m_TextureSlots;
        uint32_t m_TextureSlotIndex = 1;
        uint32_t m_VertexCount = 0;
        uint32_t m_IndexCount = 0;
        uint32_t m_TextureCount = 0;
    };
}