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

        inline void AddInitPass(const Ref<IDispatchPass>& pass) { init_passes_.push_back(pass); }
        inline void AddDispathcPass(const Ref<IDispatchPass>& pass) { dispatch_passes_.push_back(pass); }
        inline void AddDrawPass(const Ref<IDrawPass>& pass) { draw_passes_.push_back(pass); }

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

        Ref<UI> GetUI() { return ui_; }
        Ref<PipelineState> GetCurrentPipelineState() { return current_pipeline_state_; }
        Ref<Scene> GetCurrentScene() { return current_scene_; }
        Ref<FrameBuffer> GetCurrentFrameBuffer() { return current_frame_buffer_; }
    protected:

        void InitConstants() {}
        void UpdateConstants();
        void CalculateCameraMatrix();
        void CalculateLights();

    protected:
        uint32_t frame_index_;
        uint32_t current_frame_index_;
        uint32_t max_frame_in_flight_;

        Vec<Frame> frames_;
        Vec<Ref<UniformBuffer>> ubo_draw_frame_constant_;
        Vec<Ref<UniformBuffer>> ubo_light_info_;
        Vec<Ref<UniformBuffer>> ubo_draw_batch_constant_;
        Vec<Ref<UniformBuffer>> ubo_shadow_matrices_constant_;

        Vec<Ref<IDispatchPass>> init_passes_;
        Vec<Ref<IDispatchPass>> dispatch_passes_;
        Vec<Ref<IDrawPass>> draw_passes_;

        UMap<String, Ref<FrameBuffer>> frame_buffers_;

        Ref<UI> ui_ = nullptr;
        Ref<PipelineState> current_pipeline_state_ = nullptr;
        Ref<Scene> current_scene_ = nullptr;
        Ref<Shader> current_shader_ = nullptr;
        Ref<FrameBuffer> current_frame_buffer_ = nullptr;
    };

    GraphicsManager *GetGraphicsManager();
    extern GraphicsManager *g_GraphicsManager;
} // namespace Rocket