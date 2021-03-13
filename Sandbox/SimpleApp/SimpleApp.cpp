#include "SimpleApp.h"
#include "Module/MemoryManager.h"
#include "Module/AssetLoader.h"
#include "Module/AudioManager.h"
#include "Module/WindowManager.h"
#include "Module/GraphicsManager.h"
#include "Module/PipelineStateManager.h"
#include "Module/ProcessManager.h"
#include "Module/EventManager.h"
#include "Module/SceneManager.h"
#include "Module/GameLogic.h"

#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Scene/Component/EditorCamera.h"
#include "Scene/Component/SceneCamera.h"
#include "Scene/Component/PlanarMesh.h"
#include "Scene/Component/Transform.h"

#include "Logic/StateMachine.h"

#include <random>

namespace Rocket
{
    Application* g_Application;
    MemoryManager* g_MemoryManager;
    AssetLoader* g_AssetLoader;
    AudioManager* g_AudioManager;
    WindowManager* g_WindowManager;
    PipelineStateManager* g_PipelineStateManager;
    GraphicsManager* g_GraphicsManager;
    ProcessManager* g_ProcessManager;
    EventManager* g_EventManager;
    SceneManager* g_SceneManager;
    GameLogic* g_GameLogic;
    
    Application* CreateApplication()
    {
        g_Application = new SimpleApp();
        return g_Application;
    }

    void SimpleApp::PreInitializeModule()
    {
        g_MemoryManager = GetMemoryManager();
        g_AssetLoader = GetAssetLoader();
        g_ProcessManager = GetProcessManager();
        g_AudioManager = GetAudioManager();
        g_SceneManager = GetSceneManager();
        g_WindowManager = GetWindowManager();
        g_GraphicsManager = GetGraphicsManager();
        g_PipelineStateManager = GetPipelineStateManager();
        g_EventManager = GetEventManager();
        g_GameLogic = GetGameLogic();

        PushModule(g_MemoryManager);
        PushModule(g_AssetLoader);
        PushModule(g_WindowManager);
        PushModule(g_ProcessManager);
        PushModule(g_EventManager);
        
        PushModule(g_AudioManager);
        PushModule(g_SceneManager);

        PushModule(g_GraphicsManager);
        PushModule(g_PipelineStateManager);

        PushModule(g_GameLogic);
    }

    void SimpleApp::PostInitializeModule()
    {
        // Register Event Listener
        {
            bool ret = false;
            ret = g_EventManager->AddListener(
                REGISTER_DELEGATE_CLASS(Application::OnWindowClose, *g_Application), 
                EventHashTable::HashString("window_close"));
            RK_CORE_ASSERT(ret, "Register window_close Failed");
            ret = g_EventManager->AddListener(
                REGISTER_DELEGATE_CLASS(GraphicsManager::OnWindowResize, *g_GraphicsManager), 
                EventHashTable::HashString("window_resize"));
            RK_CORE_ASSERT(ret, "Register window_resize Failed");
            ret = g_EventManager->AddListener(
                REGISTER_DELEGATE_CLASS(WindowManager::OnWindowResize, *g_WindowManager), 
                EventHashTable::HashString("window_resize"));
            RK_CORE_ASSERT(ret, "Register window_resize Failed");
            ret = g_EventManager->AddListener(
                REGISTER_DELEGATE_CLASS(GameLogic::OnUIEvent, *g_GameLogic), 
                EventHashTable::HashString("ui_event_logic"));
            RK_CORE_ASSERT(ret, "Register ui_event_logic Failed");
        }

        // Setup Game Logic
        {
            Eigen::Matrix<uint64_t, 6, 6> stateTransferMatrix = Eigen::Matrix<uint64_t, 6, 6>::Zero();
            stateTransferMatrix(0, 0) = 0;
            Ref<StateMachine> stateMachine = CreateRef<StateMachine>("Robot Control");
            Ref<StateNode> init = CreateRef<StateNode>("Init");
            Ref<StateEdge> egde_11 = CreateRef<StateEdge>("Edge_11");
            init->transferFun = [](const Vec<Variant>&, const uint64_t){ 
                return nullptr; 
            };

            stateMachine->SetInitState(init);

            g_GameLogic->SetStateMachine(stateMachine);
            }
    }

    void SimpleApp::PreInitialize()
    {
        // Setup game asset
        Ref<Scene> scene = CreateRef<Scene>("Test Scene");
        Scope<SceneNode> root_node = CreateScope<SceneNode>("Root Node");
        Scope<SceneNode> cam_node = CreateScope<SceneNode>("Camera Node");
        Scope<SceneNode> mesh_node = CreateScope<SceneNode>("Mesh Node");

        Ref<SceneCamera> cam = CreateRef<SceneCamera>();

        cam->SetProjectionType(SceneCamera::ProjectionType::Orthographic);
        auto width = g_WindowManager->GetWindowWidth();
        auto height = g_WindowManager->GetWindowHeight();
        cam->SetViewportSize(width, height);
        cam->SetOrthographic(10.0f, -1.0f, 10.0f);

        //cam->SetProjectionType(SceneCamera::ProjectionType::Perspective);
        //auto width = g_WindowManager->GetWindowWidth();
        //auto height = g_WindowManager->GetWindowHeight();
        //cam->SetViewportSize(width, height);
        //cam->SetPerspective(90.0f, 0.1f, 100.0f);

        std::default_random_engine e;
        std::uniform_int_distribution<unsigned> u(0, 254);
        //Scope<PlanarMesh> mesh = CreateScope<PlanarMesh>("Mesh Component");
        //for (int i = -50; i < 50; ++i)
        //{
        //    for (int j = -50; j < 50; ++j)
        //    {
        //        mesh->AddQuad(Vector3f(i, j, 0), Vector2f(1, 1), Vector4f(float(u(e))/255.0f, float(u(e)) / 255.0f, float(u(e)) / 255.0f, 1.0f));
        //    }
        //}

        scene->SetPrimaryCamera(cam);
        scene->SetPrimaryCameraTransform(Matrix4f::Identity());

        root_node->AddChild(*cam_node);
        root_node->AddChild(*mesh_node);

        Vec<Scope<SceneComponent>> mesh_components;
        //mesh_components.push_back(std::move(mesh));
        scene->AddNode(std::move(root_node));
        //scene->SetComponents(mesh->GetType(), std::move(mesh_components));

        auto ret_1 = g_SceneManager->AddScene(scene);
        auto ret_2 = g_SceneManager->SetActiveScene("Test Scene");
    }

    void SimpleApp::PostInitialize()
    {
    }
}