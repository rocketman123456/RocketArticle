#pragma once
#include "Interface/IRuntimeModule.h"
#include "Interface/IDrawPass.h"
#include "Interface/IDispatchPass.h"
#include "Module/PipelineStateManager.h"
#include "Render/FrameStructure.h"
#include "Render/DrawBasic/Shader.h"
#include "Render/DrawBasic/FrameBuffer.h"
#include "Render/DrawBasic/VertexArray.h"
#include "Render/DrawBasic/UniformBuffer.h"
#include "Render/DrawBasic/UI.h"
#include "Math/GeomMath.h"
#include "Event/Event.h"
#include "Scene/Scene.h"

namespace Rocket
{
    class GraphicsManager : inheritance IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(GraphicsManager);
        GraphicsManager() = default;
        virtual ~GraphicsManager() = default;

        virtual int Initialize() override;
        virtual void Finalize() override;

        virtual void Tick(Timestep ts) override;

        virtual void SetPipelineState(const Ref<PipelineState>& pipelineState, const Frame& frame) = 0;

        virtual void BeginPass(const Frame& frame) {}
        virtual void EndPass(const Frame& frame) {}

        virtual void BeginCompute() {}
        virtual void EndCompute() {}

        virtual void GenerateCubeMaps() = 0;
        virtual void GenerateBRDFLUT() = 0;

        virtual void BeginFrame(const Frame& frame);
        virtual void EndFrame(const Frame& frame);

        virtual void BeginFrameBuffer(const Frame& frame) {}
        virtual void EndFrameBuffer(const Frame& frame) {}

        virtual void SetPerFrameConstants(const DrawFrameContext& context) = 0;
        virtual void SetPerBatchConstants(const DrawBatchContext& context) = 0;
        virtual void SetLightInfo(const DrawFrameContext& context) = 0;

        virtual void BeginScene(const Scene& scene);
        virtual void EndScene();

        virtual void Draw();
        virtual void Present() {}

        virtual void DrawBatch(const Frame &frame) {}
        virtual void DrawFullScreenQuad() {}

        virtual bool OnWindowResize(EventPtr& e) = 0;

        inline void AddInitPass(const Ref<IDispatchPass>& pass) { m_InitPasses.push_back(pass); }
        inline void AddDispathcPass(const Ref<IDispatchPass>& pass) { m_DispatchPasses.push_back(pass); }
        inline void AddDrawPass(const Ref<IDrawPass>& pass) { m_DrawPasses.push_back(pass); }

        // For Debug
        virtual void DrawPoint(const Point3D &point, const Vector3f &color) {}
        virtual void DrawPointSet(const Point3DSet &point_set, const Vector3f &color) {}
        virtual void DrawPointSet(const Point3DSet &point_set, const Matrix4f &trans, const Vector3f &color) {}

        virtual void DrawLine(const Point3D &from, const Point3D &to, const Vector3f &color) {}
        virtual void DrawLine(const Point3DList &vertices, const Vector3f &color) {}
        virtual void DrawLine(const Point3DList &vertices, const Matrix4f &trans, const Vector3f &color) {}

        virtual void DrawTriangle(const Point3DList &vertices, const Vector3f &color) {}
        virtual void DrawTriangle(const Point3DList &vertices, const Matrix4f &trans, const Vector3f &color) {}
        virtual void DrawTriangleStrip(const Point3DList &vertices, const Vector3f &color) {}

        void DrawEdgeList(const EdgeList &edges, const Vector3f &color);
        void DrawEdgeList(const EdgeList &edges, const Matrix4f &trans, const Vector3f &color);
        void DrawPolygon(const Face &polygon, const Vector3f &color);
        void DrawPolygon(const Face &polygon, const Matrix4f &trans, const Vector3f &color);
        void DrawBox(const Vector3f &bbMin, const Vector3f &bbMax, const Vector3f &color);
        void DrawBox(const Vector3f &bbMin, const Vector3f &bbMax, const Matrix4f &trans, const Vector3f &color);

        Ref<FrameBuffer> GetFrameBuffer(const String& name);

        Ref<UI> GetUI() { return m_UI; }
        Ref<PipelineState> GetCurrentPipelineState() { return m_CurrentPipelineState; }
        Ref<Scene> GetCurrentScene() { return m_CurrentScene; }
        Ref<FrameBuffer> GetCurrentFrameBuffer() { return m_CurrentFrameBuffer; }
    protected:

        void InitConstants() {}
        void UpdateConstants();
        void CalculateCameraMatrix();
        void CalculateLights();

    protected:
        uint32_t m_FrameIndex;
        uint32_t m_CurrentFrameIndex;
        uint32_t m_MaxFrameInFlight;

        Vec<Frame> m_Frames;
        Vec<Ref<UniformBuffer>> m_uboDrawFrameConstant;
        Vec<Ref<UniformBuffer>> m_uboLightInfo;
        Vec<Ref<UniformBuffer>> m_uboDrawBatchConstant;
        Vec<Ref<UniformBuffer>> m_uboShadowMatricesConstant;

        Vec<Ref<IDispatchPass>> m_InitPasses;
        Vec<Ref<IDispatchPass>> m_DispatchPasses;
        Vec<Ref<IDrawPass>> m_DrawPasses;

        UMap<String, Ref<FrameBuffer>> m_FrameBuffers;

        Ref<UI> m_UI = nullptr;
        Ref<PipelineState> m_CurrentPipelineState = nullptr;
        Ref<Scene> m_CurrentScene = nullptr;
        Ref<Shader> m_CurrentShader = nullptr;
        Ref<FrameBuffer> m_CurrentFrameBuffer = nullptr;
    };

    GraphicsManager *GetGraphicsManager();
    extern GraphicsManager *g_GraphicsManager;
} // namespace Rocket