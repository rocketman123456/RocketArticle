#pragma once
#include "Interface/IRuntimeModule.h"
#include "Interface/IDrawPass.h"
#include "Interface/IDispatchPass.h"
#include "Interface/IPipelineStateManager.h"
#include "Interface/IEvent.h"
#include "Common/GeomMath.h"
#include "Scene/Scene.h"

#include <map>

#define MAX_FRAME_IN_FLIGHT 2

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

        virtual void SetPipelineState(const Ref<PipelineState> &pipelineState, const Frame &frame) {}

        virtual void BeginPass(const Frame &frame) {}
        virtual void EndPass(const Frame &frame) {}

        virtual void BeginCompute() {}
        virtual void EndCompute() {}

        virtual void BeginFrame(const Frame &frame);
        virtual void EndFrame(const Frame &frame);

        virtual void BeginScene(const Scene &scene);
        virtual void EndScene();

        virtual void BeginShadowMap(const int32_t light_index, const texture_id &shadowmap, const Frame &frame) {}
        virtual void EndShadowMap(const texture_id &shadowmap) {}
        virtual void SetShadowMaps(const Frame &frame) {}

        virtual void GenerateTexture(const char *id, const uint32_t width, const uint32_t height) {}
        //virtual void CreateTexture(SceneObjectTexture& texture) {}
        virtual void ReleaseTexture(texture_id &texture) {}
        virtual void BeginRenderToTexture(int32_t &context, const int32_t texture, const uint32_t width, const uint32_t height) {}
        virtual void EndRenderToTexture(int32_t &context) {}

        virtual void Draw();
        virtual void Present() {}

        virtual void DrawBatch(const Frame &frame) {}
        virtual void DrawFullScreenQuad() {}

        virtual bool OnWindowResize(EventPtr& e) = 0;

        inline void AddInitPass(const Ref<IDispatchPass>& pass) { m_InitPasses.push_back(pass); }
        inline void AddDispathcPass(const Ref<IDispatchPass>& pass) { m_DispatchPasses.push_back(pass); }
        inline void AddDrawPass(const Ref<IDrawPass>& pass) { m_DrawPasses.push_back(pass); }

        virtual texture_id GenerateCubeShadowMapArray(const uint32_t width, const uint32_t height, const uint32_t count)
        {
            texture_id id;
            return id;
        }

        virtual texture_id GenerateShadowMapArray(const uint32_t width, const uint32_t height, const uint32_t count)
        {
            texture_id id;
            return id;
        }

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

    protected:
        virtual void SwapBuffers() = 0;

        void InitConstants() {}
        void UpdateConstants();
        void CalculateCameraMatrix();
        void CalculateLights();

    protected:
        uint32_t m_nFrameIndex;
        std::array<Frame, MAX_FRAME_IN_FLIGHT> m_Frames;
        Vec<Ref<IDispatchPass>> m_InitPasses;
        Vec<Ref<IDispatchPass>> m_DispatchPasses;
        Vec<Ref<IDrawPass>> m_DrawPasses;
        std::map<std::string, texture_id> m_Textures;
    };

    GraphicsManager *GetGraphicsManager();
    extern GraphicsManager *g_GraphicsManager;
} // namespace Rocket