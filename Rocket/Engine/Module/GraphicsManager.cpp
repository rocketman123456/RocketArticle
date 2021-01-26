#include "Module/GraphicsManager.h"
#include "Module/Application.h"

namespace Rocket
{
    int GraphicsManager::Initialize()
    {
        return 0;
    }

    void GraphicsManager::Finalize()
    {
    }

    void GraphicsManager::Tick(Timestep ts)
    {
        UpdateConstants();

        BeginFrame(m_Frames[m_nFrameIndex]);
        Draw();
        EndFrame(m_Frames[m_nFrameIndex]);

        Present();
    }

    void GraphicsManager::UpdateConstants()
    {
        auto& frame = m_Frames[m_nFrameIndex];

        for (auto& pDbc : frame.batchContexts)
        {
        }
    }

    void GraphicsManager::Draw()
    {
        auto& frame = m_Frames[m_nFrameIndex];

        for (auto& pDispatchPass : m_DispatchPasses)
        {
            pDispatchPass->BeginPass(frame);
            pDispatchPass->Dispatch(frame);
            pDispatchPass->EndPass(frame);
        }

        for (auto& pDrawPass : m_DrawPasses)
        {
            pDrawPass->BeginPass(frame);
            pDrawPass->Draw(frame);
            pDrawPass->EndPass(frame);
        }
    }

    void GraphicsManager::BeginScene(const Scene& scene)
    {
        // first, call init passes on frame 0
        for (const auto& pPass : m_InitPasses)
        {
            pPass->BeginPass(m_Frames[0]);
            pPass->Dispatch(m_Frames[0]);
            pPass->EndPass(m_Frames[0]);
        }

        auto config = g_Application->GetConfig();

        for (int32_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++)
        {
            m_Frames[i] = m_Frames[0];
            m_Frames[i].frameIndex = i;

            // generate shadow map array
            if (m_Frames[i].frameContext.shadowMap.texture == -1) {
                m_Frames[i].frameContext.shadowMap =
                    g_GraphicsManager->GenerateShadowMapArray(
                        config->GetConfigInfo<uint32_t>("Graphics", "shadow_map_width"),
                        config->GetConfigInfo<uint32_t>("Graphics", "shadow_map_height"),
                        config->GetConfigInfo<uint32_t>("Graphics", "max_shadow_map_count"));
            }

            // generate global shadow map array
            if (m_Frames[i].frameContext.globalShadowMap.texture == -1) {
                m_Frames[i].frameContext.globalShadowMap =
                    g_GraphicsManager->GenerateShadowMapArray(
                        config->GetConfigInfo<uint32_t>("Graphics", "global_shadow_map_width"),
                        config->GetConfigInfo<uint32_t>("Graphics", "global_shadow_map_height"),
                        config->GetConfigInfo<uint32_t>("Graphics", "max_global_shadow_map_count"));
            }

            // generate cube shadow map array
            if (m_Frames[i].frameContext.cubeShadowMap.texture == -1) {
                m_Frames[i].frameContext.cubeShadowMap =
                    g_GraphicsManager->GenerateCubeShadowMapArray(
                        config->GetConfigInfo<uint32_t>("Graphics", "cube_shadow_map_width"),
                        config->GetConfigInfo<uint32_t>("Graphics", "cube_shadow_map_height"),
                        config->GetConfigInfo<uint32_t>("Graphics", "max_cube_shadow_map_count"));
            }
        }
    }

    void GraphicsManager::EndScene(const Scene& scene)
    {
        for (auto& item : m_Textures)
        {
        }
        m_Textures.clear();
    }

    void GraphicsManager::BeginFrame(const Frame& frame) 
    {
    }

    void GraphicsManager::EndFrame(const Frame& frame) 
    { 
        m_nFrameIndex = (m_nFrameIndex + 1) % MAX_FRAME_IN_FLIGHT; 
    }

    void GraphicsManager::DrawEdgeList(const EdgeList& edges, const Vector3f& color)
    {
        Point3DList point_list;
        for (const auto& edge : edges)
        {
            point_list.push_back(edge->first);
            point_list.push_back(edge->second);
        }
        DrawLine(point_list, color);
    }

    void GraphicsManager::DrawEdgeList(const EdgeList& edges, const Matrix4f& trans, const Vector3f& color)
    {
        Point3DList point_list;
        for (const auto& edge : edges)
        {
            point_list.push_back(edge->first);
            point_list.push_back(edge->second);
        }
        DrawLine(point_list, trans, color);
    }

    void GraphicsManager::DrawPolygon(const Face& polygon, const Vector3f& color)
    {
        Point3DSet vertices;
        Point3DList edges;
        for (const auto& pEdge : polygon.Edges)
        {
            vertices.insert({pEdge->first, pEdge->second});
            edges.push_back(pEdge->first);
            edges.push_back(pEdge->second);
        }
        DrawLine(edges, color);
        DrawPointSet(vertices, color);
        DrawTriangle(polygon.GetVertices(), color * 0.5f);
    }

    void GraphicsManager::DrawPolygon(const Face& polygon, const Matrix4f& trans, const Vector3f& color)
    {
        Point3DSet vertices;
        Point3DList edges;
        for (const auto& pEdge : polygon.Edges)
        {
            vertices.insert({pEdge->first, pEdge->second});
            edges.push_back(pEdge->first);
            edges.push_back(pEdge->second);
        }
        DrawLine(edges, trans, color);
        DrawPointSet(vertices, trans, color);
        DrawTriangle(polygon.GetVertices(), trans, color * 0.5f);
    }

    void GraphicsManager::DrawBox(const Vector3f& bbMin, const Vector3f& bbMax, const Vector3f& color)
    {
        //  ******0--------3********
        //  *****/:       /|********
        //  ****1--------2 |********
        //  ****| :      | |********
        //  ****| 4- - - | 7********
        //  ****|/       |/*********
        //  ****5--------6**********

        // vertices
        Point3DPtr points[8];
        for (auto& point : points) point = Ref<Point3D>(new Point3D(bbMin));
        *points[0] = *points[2] = *points[3] = *points[7] = bbMax;
        (*points[0].get())[0] = bbMin[0];
        (*points[2].get())[1] = bbMin[1];
        (*points[7].get())[2] = bbMin[2];
        (*points[1].get())[2] = bbMax[2];
        (*points[4].get())[1] = bbMax[1];
        (*points[6].get())[0] = bbMax[0];

        // edges
        EdgeList edges;

        // top
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[0], points[3]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[3], points[2]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[2], points[1]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[1], points[0]))));

        // bottom
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[4], points[7]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[7], points[6]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[6], points[5]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[5], points[4]))));

        // side
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[0], points[4]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[1], points[5]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[2], points[6]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[3], points[7]))));

        DrawEdgeList(edges, color);
    }

    void GraphicsManager::DrawBox(const Vector3f& bbMin, const Vector3f& bbMax, const Matrix4f& trans, const Vector3f& color)
    {
        //  ******0--------3********
        //  *****/:       /|********
        //  ****1--------2 |********
        //  ****| :      | |********
        //  ****| 4- - - | 7********
        //  ****|/       |/*********
        //  ****5--------6**********

        // vertices
        Point3DPtr points[8];
        for (auto& point : points) point = std::shared_ptr<Point3D>(new Point3D(bbMin));
        *points[0] = *points[2] = *points[3] = *points[7] = bbMax;
        (*points[0].get())[0] = bbMin[0];
        (*points[2].get())[1] = bbMin[1];
        (*points[7].get())[2] = bbMin[2];
        (*points[1].get())[2] = bbMax[2];
        (*points[4].get())[1] = bbMax[1];
        (*points[6].get())[0] = bbMax[0];

        // edges
        EdgeList edges;

        // top
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[0], points[3]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[3], points[2]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[2], points[1]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[1], points[0]))));

        // bottom
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[4], points[7]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[7], points[6]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[6], points[5]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[5], points[4]))));

        // side
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[0], points[4]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[1], points[5]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[2], points[6]))));
        edges.push_back(Ref<Edge>(new Edge(make_pair(points[3], points[7]))));

        DrawEdgeList(edges, trans, color);
    }
}