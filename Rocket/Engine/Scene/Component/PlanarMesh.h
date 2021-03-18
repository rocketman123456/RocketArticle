#pragma once
#include "Scene/Component/Model.h"
#include "Render/DrawBasic/Texture.h"
#include "Math/GeomMath.h"

namespace Rocket
{
#pragma pack(push)
#pragma pack(4)
    struct QuadVertex
	{
		Vector4f Position;  // 16 bytes
		Vector4f Color;     // 16 bytes
		Vector2f TexCoord;  // 8 bytes
		float TexIndex;     // 4 bytes
		float TilingFactor; // 4 bytes
	};                      // total 48 bytes
#pragma pack(pop)

    using QuadIndex = uint32_t;

    // This is mainly for debug and develop purpose
    class PlanarMesh : implements Mesh
    {
    public:
        COMPONENT(PlanarMesh);
    public:
        static const uint32_t m_MaxTexture = 16;
        static Vector4f s_QuadVertexPositions[4];
    public:
        PlanarMesh(const String& name) : m_Name(name) 
        { 
            Ref<Texture2D> WhiteTexture = Texture2D::Create(2, 2);
            uint32_t whiteTextureData[] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
            WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t) * 4);
            m_TextureSlots.resize(m_MaxTexture);
            for(int i = 0; i < m_MaxTexture; ++i)
            {
                m_TextureSlots[i] = nullptr;
            }
            m_TextureSlots[0] = WhiteTexture;
        }
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
        void Invalidate() { m_MeshChange = false; }
        bool GetChangeState() { return m_MeshChange; }
        void SetChangeState(bool state) { m_MeshChange = state; }
        uint32_t GetTextureCount() { return m_TextureCount; }
    private:
        uint32_t m_TextureSlotIndex = 1;
        uint32_t m_VertexCount = 0;
        uint32_t m_IndexCount = 0;
        uint32_t m_IndexOffset = 0;
        uint32_t m_TextureCount = 1; // default white texture
        bool     m_MeshChange = false;

        String m_Name;
        Vec<QuadVertex> m_Vertex;
        Vec<QuadIndex> m_Index;
        Vec<Ref<Texture2D>> m_TextureSlots;
    };
}
