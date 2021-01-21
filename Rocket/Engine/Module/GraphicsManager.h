#pragma once
#include "Interface/IRuntimeModule.h"
#include "Common/GeomMath.h"

namespace Rocket
{
    class GraphicsManager : inheritance IRuntimeModule
    {
    public:
        GraphicsManager(const std::string &name = "GraphicsManager") : IRuntimeModule(name) {}
        virtual ~GraphicsManager() = default;

        // For Debug
        [[maybe_unused]] virtual void DrawPoint(const Point3D& point, const Vector3f& color);
        [[maybe_unused]] virtual void DrawPointSet(const Point3DSet& point_set, const Vector3f& color);
        [[maybe_unused]] virtual void DrawPointSet(const Point3DSet& point_set, const Matrix4f& trans, const Vector3f& color);

        [[maybe_unused]] virtual void DrawLine(const Point3D& from, const Point3D& to, const Vector3f &color);
        [[maybe_unused]] virtual void DrawLine(const Point3DList& vertices, const Vector3f &color);
        [[maybe_unused]] virtual void DrawLine(const Point3DList& vertices, const Matrix4f& trans, const Vector3f &color);

        [[maybe_unused]] virtual void DrawTriangle(const Point3DList& vertices, const Vector3f &color);
        [[maybe_unused]] virtual void DrawTriangle(const Point3DList& vertices, const Matrix4f& trans, const Vector3f &color);
        [[maybe_unused]] virtual void DrawTriangleStrip(const Point3DList& vertices, const Vector3f &color);

        [[maybe_unused]] void DrawEdgeList(const EdgeList& edges, const Vector3f& color);
        [[maybe_unused]] void DrawPolygon(const Face& face, const Vector3f& color);
        [[maybe_unused]] void DrawPolygon(const Face& face, const Matrix4f& trans, const Vector3f& color);
        [[maybe_unused]] void DrawBox(const Vector3f& bbMin, const Vector3f& bbMax, const Vector3f& color);
        [[maybe_unused]] void DrawBox(const Vector3f& bbMin, const Vector3f& bbMax, const Matrix4f& trans, const Vector3f& color);

    protected:
        virtual void SwapBuffers() = 0;
    };

    GraphicsManager* GetGraphicsManager();
    extern GraphicsManager* g_GraphicsManager;
}