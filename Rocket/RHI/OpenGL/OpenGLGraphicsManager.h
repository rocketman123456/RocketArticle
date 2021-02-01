#pragma once
#include "Module/GraphicsManager.h"
#include "Render/DrawBasic/Shader.h"

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

        void BeginFrame(const Frame& frame) override;
        void EndFrame(const Frame& frame) override;

        void SetPerFrameConstants(const DrawFrameContext& context);
        void SetPerBatchConstants(const DrawBatchContext& context);
        void SetLightInfo(const LightInfo& lightInfo);

        void BeginScene(const Scene& scene) final { GraphicsManager::BeginScene(scene); }
        void EndScene() final;

        void GenerateTexture(const char* id, const uint32_t width, const uint32_t height) final {}
        //void CreateTexture(SceneObjectTexture& texture) final {}
        void ReleaseTexture(texture_id& texture) final {}
        void BeginRenderToTexture(int32_t& context, const int32_t texture, const uint32_t width, const uint32_t height) final {}
        void EndRenderToTexture(int32_t& context) final {}

        texture_id GenerateCubeShadowMapArray(const uint32_t width, const uint32_t height, const uint32_t count) final;
        texture_id GenerateShadowMapArray(const uint32_t width, const uint32_t height, const uint32_t count) final;

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
    
        Ref<Shader> m_CurrentShader;
        
        uint32_t m_ShadowMapFramebufferName;
        uint32_t m_uboDrawFrameConstant[MAX_FRAME_IN_FLIGHT] = {0};
        uint32_t m_uboLightInfo[MAX_FRAME_IN_FLIGHT] = {0};
        uint32_t m_uboDrawBatchConstant[MAX_FRAME_IN_FLIGHT] = {0};
        uint32_t m_uboShadowMatricesConstant[MAX_FRAME_IN_FLIGHT] = {0};
        uint32_t m_uboDebugConstant[MAX_FRAME_IN_FLIGHT] = {0};

        struct QuadVertex
        {
            Vector3f Position;
            Vector4f Color;
            Vector2f TexCoord;
            float TexIndex;
            float TilingFactor;
        };

        struct OpenGL2DBatchContext : public DrawBatchContext, QuadVertex
        {
        };

        struct OpenGLDrawBatchContext : public DrawBatchContext
        {
            uint32_t vao{0};
            uint32_t mode{0};
            uint32_t type{0};
            int32_t count{0};
        };

        struct DebugDrawBatchContext : public OpenGLDrawBatchContext
        {
            Vector3f color;
            Matrix4f trans;
        };

        std::vector<uint32_t> m_Buffers;

        std::vector<DebugDrawBatchContext> m_DebugDrawBatchContext;
        std::vector<uint32_t> m_DebugBuffers;

        OpenGL2DBatchContext m_Draw2DContext;

        OpenGLDrawBatchContext m_SkyBoxDrawBatchContext;
        OpenGLDrawBatchContext m_TerrainDrawBatchContext;
    };
}