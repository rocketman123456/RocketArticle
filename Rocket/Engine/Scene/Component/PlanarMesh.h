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
		Vector4f position;  // 16 bytes
		Vector4f color;     // 16 bytes
		Vector2f tex_coord;  // 8 bytes
		float tex_index;     // 4 bytes
		float tiling_factor; // 4 bytes
	};                      // total 48 bytes
#pragma pack(pop)

    using QuadIndex = uint32_t;

    // This is mainly for debug and develop purpose
    class PlanarMesh : implements Mesh
    {
    public:
        COMPONENT(PlanarMesh);
    public:
        static const uint32_t max_texture = 16;
        static Vector4f quad_vertex_positions[4];
    public:
        PlanarMesh(const String& name) : name_(name) 
        { 
            Ref<Texture2D> WhiteTexture = Texture2D::Create(2, 2);
            uint32_t whiteTextureData[] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
            WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t) * 4);
            texture_slots_.resize(max_texture);
            for(int i = 0; i < max_texture; ++i)
            {
                texture_slots_[i] = nullptr;
            }
            texture_slots_[0] = WhiteTexture;
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

        Vec<QuadVertex>& GetVertex() { return vertex_; }
        Vec<QuadIndex>& GetIndex() { return index_; }
        Vec<Ref<Texture2D>>& GetTexture() { return texture_slots_; }
        void Invalidate() { mesh_change_ = false; }
        bool GetChangeState() { return mesh_change_; }
        void SetChangeState(bool state) { mesh_change_ = state; }
        uint32_t GetTextureCount() { return texture_count_; }
    private:
        uint32_t texture_slot_index_ = 1;
        uint32_t vertex_count_ = 0;
        uint32_t index_count_ = 0;
        uint32_t index_offset_ = 0;
        uint32_t texture_count_ = 1; // default white texture
        bool     mesh_change_ = false;

        String name_;
        Vec<QuadVertex> vertex_;
        Vec<QuadIndex> index_;
        Vec<Ref<Texture2D>> texture_slots_;
    };
}
