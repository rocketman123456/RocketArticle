#pragma once
#include "Module/GraphicsManager.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "OpenGL/OpenGLVertexBuffer.h"
#include "OpenGL/OpenGLIndexBuffer.h"
#include "OpenGL/OpenGLShader.h"
#include "OpenGL/OpenGLTexture.h"

struct GLFWwindow;

namespace Rocket
{
    struct QuadVertex
    {
        Vector3f Position;
        Vector4f Color;
        Vector2f TexCoord;
        float TexIndex;
        float TilingFactor;
    };

    struct Renderer2DStatistics
    {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;

        uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
        uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
    };

    struct Renderer2DData
    {
        static const uint32_t MaxQuads = 40'000;
        static const uint32_t MaxVertices = MaxQuads * 4;
        static const uint32_t MaxIndices = MaxQuads * 6;
        static const uint32_t MaxTextureSlots = 16;

        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader> TextureShader;
        Ref<Texture2D> WhiteTexture;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture

        Vector4f QuadVertexPositions[4];

        Renderer2DStatistics Stats;
    };

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
        void StartBatch();
        void Flush();
        void NextBatch()
        {
            Flush();
            StartBatch();
        }
        void DrawQuad(const Matrix4f& transform, const Vector4f& color);

    protected:
        void SwapBuffers() final;
        bool Resize(int32_t width, int32_t height);

    private:
        GLFWwindow* m_WindowHandle = nullptr;
        bool m_VSync = true;

        struct OpenGL2DBatchContext : public DrawBatchContext
        {
            Renderer2DData data;
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