#include "Module/GraphicsManager.h"
#include "Module/Application.h"
#include "Module/SceneManager.h"
#include "Render/DrawPass/ForwardGeometryPass.h"
#include "Render/DispatchPass/BRDFGenerate.h"
#include "Render/DispatchPass/SkyBoxGenerate.h"

using namespace Rocket;

int GraphicsManager::Initialize()
{
    // Add Init Pass
    init_passes_.push_back(CreateRef<BRDFGenerate>());
    //init_passes_.push_back(CreateRef<SkyBoxGenerate>());

    // Get Max Frame In Flight
    auto& config = g_Application->GetConfig();
    max_frame_in_flight_ = config->GetConfigInfo<uint32_t>("Graphics", "max_frame_in_flight");

    // Add Draw Pass
    draw_passes_.push_back(CreateRef<ForwardGeometryPass>());

    // Init Frames Data
    frames_.resize(max_frame_in_flight_);
    for(size_t i = 0; i < max_frame_in_flight_; ++i)
    {
        frames_[i] = {};
    }

    // Init UBOs
    ubo_draw_frame_constant_.resize(max_frame_in_flight_);
    ubo_light_info_.resize(max_frame_in_flight_);
    ubo_draw_batch_constant_.resize(max_frame_in_flight_);
    ubo_shadow_matrices_constant_.resize(max_frame_in_flight_);
    for(size_t i = 0; i < max_frame_in_flight_; ++i)
    {
        ubo_draw_frame_constant_[i] = nullptr;
        ubo_draw_batch_constant_[i] = nullptr;
        ubo_light_info_[i] = nullptr;
        ubo_shadow_matrices_constant_[i] = nullptr;
    }

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
    else if (!current_scene_ || current_scene_->GetSceneChange() || current_scene_ != scene)
    {
        EndScene();
        current_scene_ = g_SceneManager->GetActiveScene();
        BeginScene(*current_scene_);
        current_scene_->SetSceneChange(false);
    }
    
    UpdateConstants();

    BeginFrame(frames_[current_frame_index_]);
    Draw();
    EndFrame(frames_[current_frame_index_]);

    Present();
}

void GraphicsManager::UpdateConstants()
{
    auto& frame = frames_[current_frame_index_];

    for (auto& pDbc : frame.batch_contexts)
    {
        // TODO : implements scene update
        //current_scene_->Update();
    }

    CalculateCameraMatrix();
    CalculateLights();
}

void GraphicsManager::CalculateCameraMatrix()
{
    auto& scene = g_SceneManager->GetActiveScene();
    auto& camera = scene->GetPrimaryCamera();
    
    frames_[current_frame_index_].frame_context.projection_mat = camera->GetProjection();
    frames_[current_frame_index_].frame_context.view_mat = scene->GetPrimaryCameraTransform().inverse();
    frames_[current_frame_index_].frame_context.cam_position = scene->GetPrimaryCameraTransform().block<4, 1>(0, 3);
}

void GraphicsManager::CalculateLights()
{
}

void GraphicsManager::Draw()
{
    auto& frame = frames_[current_frame_index_];

    for (auto& pDispatchPass : dispatch_passes_)
    {
        pDispatchPass->BeginPass(frame);
        pDispatchPass->Dispatch(frame);
        pDispatchPass->EndPass(frame);
    }

    for (auto& pDrawPass : draw_passes_)
    {
        pDrawPass->BeginPass(frame);
        pDrawPass->Draw(frame);
        pDrawPass->EndPass(frame);
    }
}

void GraphicsManager::BeginScene(const Scene& scene)
{
    // first, call init passes on frame 0
    for (const auto& pPass : init_passes_)
    {
        pPass->BeginPass(frames_[0]);
        pPass->Dispatch(frames_[0]);
        pPass->EndPass(frames_[0]);
    }

    auto config = g_Application->GetConfig();

    for (uint32_t i = 0; i < max_frame_in_flight_; i++)
    {
        frames_[i] = frames_[0];
        frames_[i].frame_index = i;
    }
}

void GraphicsManager::EndScene()
{
    frames_.clear();
    frames_.resize(max_frame_in_flight_);

    // Clear Buffers
    //ubo_draw_frame_constant_.clear();
    //ubo_light_info_.clear();
    //ubo_draw_batch_constant_.clear();
    //ubo_shadow_matrices_constant_.clear();

    // Regenerate Buffers
    //ubo_draw_frame_constant_.resize(max_frame_in_flight_);
    //ubo_light_info_.resize(max_frame_in_flight_);
    //ubo_draw_batch_constant_.resize(max_frame_in_flight_);
    //ubo_shadow_matrices_constant_.resize(max_frame_in_flight_);
    
    //for(size_t i = 0; i < max_frame_in_flight_; ++i)
    //{
    //    ubo_draw_frame_constant_[i] = nullptr;
    //    ubo_light_info_[i] = nullptr;
    //    ubo_draw_batch_constant_[i] = nullptr;
    //    ubo_shadow_matrices_constant_[i] = nullptr;
    //}

    current_scene_ = nullptr;
    //current_shader_ = nullptr;
    //current_frame_buffer_ = nullptr;
}

void GraphicsManager::BeginFrame(const Frame& frame) 
{
}

void GraphicsManager::EndFrame(const Frame& frame) 
{ 
    current_frame_index_ = (current_frame_index_ + 1) % max_frame_in_flight_; 
}

Ref<FrameBuffer> GraphicsManager::GetFrameBuffer(const String& name)
{ 
    auto it = frame_buffers_.find(name);
    if (it == frame_buffers_.end())
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
