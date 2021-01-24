#pragma once
#include "Interface/IRuntimeModule.h"
#include "Interface/IDrawPass.h"
#include "Interface/IDispatchPass.h"
#include "Common/GeomMath.h"
#include "Scene/Scene.h"

namespace Rocket
{
    class GraphicsManager : inheritance IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(GraphicsManager);
        GraphicsManager() = default;
        virtual ~GraphicsManager() = default;

        //virtual int Initialize() override;
        //virtual void Finalize() override;

        //virtual void Tick(Timestep ts) override;

        virtual void BeginPass() {}
        virtual void EndPass() {}

        virtual void BeginCompute() {}
        virtual void EndCompute() {}

        virtual void BeginFrame(const Frame& frame) {}
        virtual void EndFrame(const Frame& frame) {}

        virtual void BeginScene(const Scene& scene) {}
        virtual void EndScene() { m_Frames.clear(); }

        // For Debug
        virtual void DrawPoint(const Point3D& point, const Vector3f& color);
        virtual void DrawPointSet(const Point3DSet& point_set, const Vector3f& color);
        virtual void DrawPointSet(const Point3DSet& point_set, const Matrix4f& trans, const Vector3f& color);

        virtual void DrawLine(const Point3D& from, const Point3D& to, const Vector3f &color);
        virtual void DrawLine(const Point3DList& vertices, const Vector3f &color);
        virtual void DrawLine(const Point3DList& vertices, const Matrix4f& trans, const Vector3f &color);

        virtual void DrawTriangle(const Point3DList& vertices, const Vector3f &color);
        virtual void DrawTriangle(const Point3DList& vertices, const Matrix4f& trans, const Vector3f &color);
        virtual void DrawTriangleStrip(const Point3DList& vertices, const Vector3f &color);

        void DrawEdgeList(const EdgeList& edges, const Vector3f& color);
        void DrawPolygon(const Face& face, const Vector3f& color);
        void DrawPolygon(const Face& face, const Matrix4f& trans, const Vector3f& color);
        void DrawBox(const Vector3f& bbMin, const Vector3f& bbMax, const Vector3f& color);
        void DrawBox(const Vector3f& bbMin, const Vector3f& bbMax, const Matrix4f& trans, const Vector3f& color);

    protected:
        virtual void SwapBuffers() = 0;

    protected:
        std::vector<Frame> m_Frames;
        std::vector<Ref<IDispatchPass>> m_InitPasses;
        std::vector<Ref<IDrawPass>> m_DrawPasses;
    };

    GraphicsManager* GetGraphicsManager();
    extern GraphicsManager* g_GraphicsManager;
}