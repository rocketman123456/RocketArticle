#pragma once
#include "Module/GraphicsManager.h"
#include "OpenGL/OpenGLPipelineStateManager.h"
#include "OpenGL/OpenGLFrameBuffer.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "OpenGL/OpenGLVertexBuffer.h"
#include "OpenGL/OpenGLIndexBuffer.h"
#include "OpenGL/OpenGLUniformBuffer.h"
#include "OpenGL/OpenGLShader.h"
#include "OpenGL/OpenGLTexture.h"

struct GLFWwindow;

namespace Rocket
{
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

        void BeginFrame(const Frame& frame) final;
        void EndFrame(const Frame& frame) final;

        void BeginFrameBuffer(const Frame& frame) final;
        void EndFrameBuffer(const Frame& frame) final;

        void SetPerFrameConstants(const DrawFrameContext& context) final;
        void SetLightInfo(const DrawFrameContext& context) final;
        void SetPerBatchConstants(const DrawBatchContext& context) final;

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
        int32_t m_Width;
        int32_t m_Height;

        struct OpenGLDrawBatchContext : public DrawBatchContext
        {
            Ref<OpenGLVertexArray> VAO;
            Vec<Ref<Texture2D>>* Textures;
            uint32_t Mode = 0;
            uint32_t Type = 0;
            uint32_t MaxTextures = 0;
        };

        struct DebugDrawBatchContext : public OpenGLDrawBatchContext
        {
            Vector3f color;
            Matrix4f trans;
        };

        OpenGLDrawBatchContext m_DrawContext;
        OpenGLDrawBatchContext m_DebugContext;
    };
}