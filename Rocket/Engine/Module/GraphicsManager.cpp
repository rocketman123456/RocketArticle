#include "Module/GraphicsManager.h"
#include "Module/Application.h"
#include "Module/SceneManager.h"
#include "Module/MemoryManager.h"
#include "Render/DrawPass/ForwardGeometryPass.h"

using namespace Rocket;

int GraphicsManager::Initialize()
{
    // Get Max Frame In Flight
    auto& config = g_Application->GetConfig();
    m_MaxFrameInFlight = config->GetConfigInfo<uint32_t>("Graphics", "max_frame_in_flight");

    // Init Frames Data
    m_Frames.resize(m_MaxFrameInFlight);
    for(size_t i = 0; i < m_MaxFrameInFlight; ++i)
    {
        m_Frames[i] = Frame();
    }

    // Init UBOs
    m_uboDrawFrameConstant.resize(m_MaxFrameInFlight);
    m_uboLightInfo.resize(m_MaxFrameInFlight);
    m_uboDrawBatchConstant.resize(m_MaxFrameInFlight);
    m_uboShadowMatricesConstant.resize(m_MaxFrameInFlight);
    for(size_t i = 0; i < m_MaxFrameInFlight; ++i)
    {
        m_uboDrawFrameConstant[i] = nullptr;
        m_uboLightInfo[i] = nullptr;
        m_uboDrawBatchConstant[i] = nullptr;
        m_uboShadowMatricesConstant[i] = nullptr;
    }

    // Add Draw Pass
    m_DrawPasses.push_back(std::make_shared<ForwardGeometryPass>());

    InitConstants();
    return 0;
}

void GraphicsManager::Finalize()
{
    EndScene();
}

void GraphicsManager::Tick(Timestep ts)
{
    auto scene = g_SceneManager->GetActiveScene();
    if(!scene)
    {
        RK_GRAPHICS_WARN("No Active Scene");
    }
    else if (!m_CurrentScene || m_CurrentScene->GetSceneChange() || m_CurrentScene != scene)
    {
        EndScene();
        m_CurrentScene = g_SceneManager->GetActiveScene();
        BeginScene(*m_CurrentScene);
        m_CurrentScene->SetSceneChange(false);
    }
    
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
        // TODO : implements scene update
        //m_CurrentScene->Update();
    }

    CalculateCameraMatrix();
    CalculateLights();
}

void GraphicsManager::CalculateCameraMatrix()
{
    auto& scene = g_SceneManager->GetActiveScene();
    auto& camera = scene->GetPrimaryCamera();
    
    m_Frames[m_nFrameIndex].frameContext.projectionMatrix = camera->GetProjection();
    m_Frames[m_nFrameIndex].frameContext.viewMatrix = scene->GetPrimaryCameraTransform().inverse();
    m_Frames[m_nFrameIndex].frameContext.camPos = scene->GetPrimaryCameraTransform().block<4, 1>(0, 3);
}

void GraphicsManager::CalculateLights()
{
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

    for (uint32_t i = 0; i < m_MaxFrameInFlight; i++)
    {
        m_Frames[i] = m_Frames[0];
        m_Frames[i].frameIndex = i;
    }
}

void GraphicsManager::EndScene()
{
    for (int i = 0; i < m_Frames.size(); i++)
    {
        auto &batchContexts = m_Frames[i].batchContexts;
        batchContexts.clear();
    }

    // Clear Buffers
    m_uboDrawFrameConstant.clear();
    m_uboLightInfo.clear();
    m_uboDrawBatchConstant.clear();
    m_uboShadowMatricesConstant.clear();

    // Regenerate Buffers
    m_uboDrawFrameConstant.resize(m_MaxFrameInFlight);
    m_uboLightInfo.resize(m_MaxFrameInFlight);
    m_uboDrawBatchConstant.resize(m_MaxFrameInFlight);
    m_uboShadowMatricesConstant.resize(m_MaxFrameInFlight);
    
    for(size_t i = 0; i < m_MaxFrameInFlight; ++i)
    {
        m_uboDrawFrameConstant[i] = nullptr;
        m_uboLightInfo[i] = nullptr;
        m_uboDrawBatchConstant[i] = nullptr;
        m_uboShadowMatricesConstant[i] = nullptr;
    }

    m_CurrentScene = nullptr;
    m_CurrentShader = nullptr;
    m_CurrentFrameBuffer = nullptr;
}

void GraphicsManager::BeginFrame(const Frame& frame) 
{
}

void GraphicsManager::EndFrame(const Frame& frame) 
{ 
    m_nFrameIndex = (m_nFrameIndex + 1) % m_MaxFrameInFlight; 
}

Ref<FrameBuffer> GraphicsManager::GetFrameBuffer(const String& name)
{ 
    auto it = m_FrameBuffers.find(name);
    if (it == m_FrameBuffers.end())
    {
        return nullptr;
    }
    else
    {
        return it->second;
    }
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
    for (auto& point : points) point = CreateRef<Point3D>(bbMin);
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
    edges.push_back(CreateRef<Edge>(make_pair(points[0], points[3])));
    edges.push_back(CreateRef<Edge>(make_pair(points[3], points[2])));
    edges.push_back(CreateRef<Edge>(make_pair(points[2], points[1])));
    edges.push_back(CreateRef<Edge>(make_pair(points[1], points[0])));

    // bottom
    edges.push_back(CreateRef<Edge>(make_pair(points[4], points[7])));
    edges.push_back(CreateRef<Edge>(make_pair(points[7], points[6])));
    edges.push_back(CreateRef<Edge>(make_pair(points[6], points[5])));
    edges.push_back(CreateRef<Edge>(make_pair(points[5], points[4])));

    // side
    edges.push_back(CreateRef<Edge>(make_pair(points[0], points[4])));
    edges.push_back(CreateRef<Edge>(make_pair(points[1], points[5])));
    edges.push_back(CreateRef<Edge>(make_pair(points[2], points[6])));
    edges.push_back(CreateRef<Edge>(make_pair(points[3], points[7])));

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
    for (auto& point : points) point = CreateRef<Point3D>(bbMin);
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
    edges.push_back(CreateRef<Edge>(make_pair(points[0], points[3])));
    edges.push_back(CreateRef<Edge>(make_pair(points[3], points[2])));
    edges.push_back(CreateRef<Edge>(make_pair(points[2], points[1])));
    edges.push_back(CreateRef<Edge>(make_pair(points[1], points[0])));

    // bottom
    edges.push_back(CreateRef<Edge>(make_pair(points[4], points[7])));
    edges.push_back(CreateRef<Edge>(make_pair(points[7], points[6])));
    edges.push_back(CreateRef<Edge>(make_pair(points[6], points[5])));
    edges.push_back(CreateRef<Edge>(make_pair(points[5], points[4])));

    // side
    edges.push_back(CreateRef<Edge>(make_pair(points[0], points[4])));
    edges.push_back(CreateRef<Edge>(make_pair(points[1], points[5])));
    edges.push_back(CreateRef<Edge>(make_pair(points[2], points[6])));
    edges.push_back(CreateRef<Edge>(make_pair(points[3], points[7])));

    DrawEdgeList(edges, trans, color);
}
