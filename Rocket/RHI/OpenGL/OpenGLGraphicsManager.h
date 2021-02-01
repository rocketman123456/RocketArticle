#pragma once
#include "Module/GraphicsManager.h"
#include "Render/DrawBasic/Shader.h"

struct GLFWwindow;

namespace Rocket
{
    struct QuadVertex;
    struct Renderer2DData;

    class OpenGLGraphicsManager : implements GraphicsManager
    {
    public:
        RUNTIME_MODULE_TYPE(OpenGLGraphicsManager);
        OpenGLGraphicsManager() = default;
        virtual ~OpenGLGraphicsManager() = default;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual void Tick(Timestep ts) final;

        void Present() final;
        void SetPipelineState(const Ref<PipelineState>& pipelineState, const Frame& frame) final;
        void DrawBatch(const Frame& frame) final;
        void DrawFullScreenQuad() final;

        void BeginFrame(const Frame& frame) override;
        void EndFrame(const Frame& frame) override;

        void SetPerFrameConstants(const DrawFrameContext& context);
        void SetPerBatchConstants(const DrawBatchContext& context);

        void BeginScene(const Scene& scene) final;
        void EndScene() final;

        // For Debug
        void DrawPoint(const Point3D& point, const Vector3f& color) final;
        void DrawPointSet(const Point3DSet& point_set, const Vector3f& color) final;
        void DrawPointSet(const Point3DSet& point_set, const Matrix4f& trans, const Vector3f& color) final;

        void DrawLine(const Point3D& from, const Point3D& to, const Vector3f &color) final;
        void DrawLine(const Point3DList& vertices, const Vector3f &color) final;
        void DrawLine(const Point3DList& vertices, const Matrix4f& trans, const Vector3f &color) final;

        void DrawTriangle(const Point3DList& vertices, const Vector3f &color) final;
        void DrawTriangle(const Point3DList& vertices, const Matrix4f& trans, const Vector3f &color) final;
        void DrawTriangleStrip(const Point3DList& vertices, const Vector3f &color) final;

        bool OnWindowResize(EventPtr& e) final;

    protected:
        void SwapBuffers() final;
        bool Resize(int32_t width, int32_t height);

    private:
        GLFWwindow* m_WindowHandle = nullptr;
        bool m_VSync = true;

        struct OpenGL2DBatchContext : public DrawBatchContext
        {
            Renderer2DData* data;
        };

        struct OpenGLDrawBatchContext : public DrawBatchContext
        {
        };

        struct DebugDrawBatchContext : public OpenGLDrawBatchContext
        {
            Vector3f color;
            Matrix4f trans;
        };

        OpenGL2DBatchContext m_Draw2DContext;
    };
}