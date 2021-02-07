#include "Scene/Component/PlanarMesh.h"

using namespace Rocket;

PlanarMesh::PlanarMesh()
{
    m_QuadVertexPositions[0] = Vector4f(-0.5f, -0.5f, 0.0f, 1.0f);
    m_QuadVertexPositions[1] = Vector4f( 0.5f, -0.5f, 0.0f, 1.0f);
    m_QuadVertexPositions[2] = Vector4f( 0.5f,  0.5f, 0.0f, 1.0f);
    m_QuadVertexPositions[3] = Vector4f(-0.5f,  0.5f, 0.0f, 1.0f);
}

PlanarMesh::PlanarMesh(const String& name)
    : m_Name(name)
{
    m_QuadVertexPositions[0] = Vector4f(-0.5f, -0.5f, 0.0f, 1.0f);
    m_QuadVertexPositions[1] = Vector4f( 0.5f, -0.5f, 0.0f, 1.0f);
    m_QuadVertexPositions[2] = Vector4f( 0.5f,  0.5f, 0.0f, 1.0f);
    m_QuadVertexPositions[3] = Vector4f(-0.5f,  0.5f, 0.0f, 1.0f);
}

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
}

void PlanarMesh::AddQuad(const Matrix4f& transform, const Ref<Texture2D>& texture, float tilingFactor, const Vector4f& tintColor)
{
}
