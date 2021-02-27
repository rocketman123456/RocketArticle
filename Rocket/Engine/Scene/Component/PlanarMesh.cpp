#include "Scene/Component/PlanarMesh.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace Rocket;

Vector4f PlanarMesh::s_QuadVertexPositions[4] = {
    Vector4f(-0.5f, -0.5f, 0.0f, 1.0f),
    Vector4f( 0.5f, -0.5f, 0.0f, 1.0f),
    Vector4f( 0.5f,  0.5f, 0.0f, 1.0f),
    Vector4f(-0.5f,  0.5f, 0.0f, 1.0f)
};

void PlanarMesh::AddQuad(const Vector2f& position, const Vector2f& size, const Vector4f& color)
{
    AddQuad(Vector3f(position[0], position[1], 0.0f), size, color);
}

void PlanarMesh::AddQuad(const Vector3f& position, const Vector2f& size, const Vector4f& color)
{
    Matrix4f transform = Matrix4f::Identity();
    transform.block<3, 1>(0, 3) = position;
    Matrix4f scale = Matrix4f::Identity();
    scale(0, 0) = size[0];
    scale(1, 1) = size[1];
    transform = transform * scale;
    AddQuad(transform, color);
}

void PlanarMesh::AddQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, float tilingFactor, const Vector4f& tintColor)
{
    AddQuad(Vector3f(position[0], position[1], 0.0f), size, texture, tilingFactor, tintColor);
}

void PlanarMesh::AddQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, float tilingFactor, const Vector4f& tintColor)
{
    Matrix4f transform = Matrix4f::Identity();
    transform.block<3, 1>(0, 3) = position;
    Matrix4f scale = Matrix4f::Identity();
    scale(0, 0) = size[0];
    scale(1, 1) = size[1];
    transform = transform * scale;
    AddQuad(transform, texture, tilingFactor, tintColor);
}

void PlanarMesh::AddRotatedQuad(const Vector2f& position, const Vector2f& size, float rotation, const Vector4f& color)
{
    AddRotatedQuad(Vector3f(position[0], position[1], 0.0f), size, rotation, color);
}

void PlanarMesh::AddRotatedQuad(const Vector3f& position, const Vector2f& size, float rotation, const Vector4f& color)
{
    AngleAxisf rot(rotation / 180.0f * M_PI, Vector3f(0, 0, 1));
    Matrix3f rot_mat = rot.matrix();
    Matrix4f transform = Matrix4f::Identity();
    transform.block<3, 1>(0, 3) = position;
    transform.block<3, 3>(0, 0) = rot_mat;
    Matrix4f scale = Matrix4f::Identity();
    scale(0, 0) = size[0];
    scale(1, 1) = size[1];
    transform = transform * scale;
    AddQuad(transform, color);
}

void PlanarMesh::AddRotatedQuad(const Vector2f& position, const Vector2f& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const Vector4f& tintColor)
{
    AddRotatedQuad(Vector3f(position[0], position[1], 0.0f), size, rotation, texture, tilingFactor, tintColor);
}

void PlanarMesh::AddRotatedQuad(const Vector3f& position, const Vector2f& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const Vector4f& tintColor)
{
    AngleAxisf rot(rotation / 180.0f * M_PI, Vector3f(0, 0, 1));
    Matrix3f rot_mat = rot.matrix();
    Matrix4f transform = Matrix4f::Identity();
    transform.block<3, 1>(0, 3) = position;
    transform.block<3, 3>(0, 0) = rot_mat;
    Matrix4f scale = Matrix4f::Identity();
    scale(0, 0) = size[0];
    scale(1, 1) = size[1];
    transform = transform * scale;
    AddQuad(transform, texture, tilingFactor, tintColor);
}

void PlanarMesh::AddQuad(const Matrix4f& transform, const Vector4f& color)
{
    constexpr size_t quadVertexCount = 4;
    const float textureIndex = 0.0f; // White Texture
    const Vector2f textureCoords[] = { 
        Vector2f(0.0f, 0.0f),
        Vector2f(1.0f, 0.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(0.0f, 1.0f),
    };
    const float tilingFactor = 1.0f;

    QuadIndex index;

    index = m_IndexOffset + 0; m_Index.push_back(index);
    index = m_IndexOffset + 1; m_Index.push_back(index);
    index = m_IndexOffset + 2; m_Index.push_back(index);

    index = m_IndexOffset + 2; m_Index.push_back(index);
    index = m_IndexOffset + 3; m_Index.push_back(index);
    index = m_IndexOffset + 0; m_Index.push_back(index);

    m_IndexOffset += 4;

    for (size_t i = 0; i < quadVertexCount; i++)
    {
        QuadVertex vertex;
        vertex.Position = transform * s_QuadVertexPositions[i];
        vertex.Color = color;
        vertex.TexCoord = textureCoords[i];
        vertex.TexIndex = textureIndex;
        vertex.TilingFactor = tilingFactor;
        m_Vertex.push_back(vertex);
    }

    m_VertexCount += 1;
    m_IndexCount += 6;
    m_MeshChange = true;
}

void PlanarMesh::AddQuad(const Matrix4f& transform, const Ref<Texture2D>& texture, float tilingFactor, const Vector4f& tintColor)
{
    constexpr size_t quadVertexCount = 4;
    const Vector2f textureCoords[] = { 
        Vector2f(0.0f, 0.0f),
        Vector2f(1.0f, 0.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(0.0f, 1.0f),
    };

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < m_TextureSlotIndex; i++)
    {
        if (*m_TextureSlots[i] == *texture)
        {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        if(m_TextureSlotIndex >= m_MaxTexture)
        {
            RK_GRAPHICS_WARN("Planar Mesh TextureSlotIndex >= MaxTexture");
            return;
        }
        textureIndex = (float)m_TextureSlotIndex;
        m_TextureSlots[m_TextureSlotIndex] = texture;
        //m_TextureSlots.push_back(texture);
        m_TextureSlotIndex++;
    }

    QuadIndex index;

    index = m_IndexOffset + 0; m_Index.push_back(index);
    index = m_IndexOffset + 1; m_Index.push_back(index);
    index = m_IndexOffset + 2; m_Index.push_back(index);

    index = m_IndexOffset + 2; m_Index.push_back(index);
    index = m_IndexOffset + 3; m_Index.push_back(index);
    index = m_IndexOffset + 0; m_Index.push_back(index);

    m_IndexOffset += 4;

    for (size_t i = 0; i < quadVertexCount; i++)
    {
        QuadVertex vertex;
        vertex.Position = transform * s_QuadVertexPositions[i];
        vertex.Color = tintColor;
        vertex.TexCoord = textureCoords[i];
        vertex.TexIndex = textureIndex;
        vertex.TilingFactor = tilingFactor;
        m_Vertex.push_back(vertex);
    }

    m_VertexCount += 1;
    m_IndexCount += 6;
    m_MeshChange = true;
}
