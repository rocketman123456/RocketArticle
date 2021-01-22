#pragma once
#include "Module/GraphicsManager.h"

struct GLFWwindow;

namespace Rocket
{
    class OpenGLGraphicsManager : implements GraphicsManager
    {
    public:
        OpenGLGraphicsManager() : GraphicsManager("OpenGLGraphicsManager") {}
        virtual ~OpenGLGraphicsManager() = default;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual void Tick(Timestep ts) final;

        //virtual void DrawPoint(const Point3D& point, const Vector3f& color) final;
        //virtual void DrawPointSet(const Point3DSet& point_set, const Vector3f& color) final;
        //virtual void DrawPointSet(const Point3DSet& point_set, const Matrix4f& trans, const Vector3f& color) final;
//
        //virtual void DrawLine(const Point3D& from, const Point3D& to, const Vector3f &color) final;
        //virtual void DrawLine(const Point3DList& vertices, const Vector3f &color) final;
        //virtual void DrawLine(const Point3DList& vertices, const Matrix4f& trans, const Vector3f &color) final;
//
        //virtual void DrawTriangle(const Point3DList& vertices, const Vector3f &color) final;
        //virtual void DrawTriangle(const Point3DList& vertices, const Matrix4f& trans, const Vector3f &color) final;
        //virtual void DrawTriangleStrip(const Point3DList& vertices, const Vector3f &color) final;

    protected:
        virtual void SwapBuffers() final;

    private:
        GLFWwindow* m_WindowHandle = nullptr;
        bool m_VSync = true;
    };
}