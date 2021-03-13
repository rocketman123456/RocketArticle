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
            ret = g_EventManager->AddListener(REGISTER_DELEGATE_CLASS(Application::OnWindowClose, *g_Application), EventHashTable::HashString("window_close"));
            RK_CORE_ASSERT(ret, "Register window_close Failed");
            ret = g_EventManager->AddListener(REGISTER_DELEGATE_CLASS(GraphicsManager::OnWindowResize, *g_GraphicsManager), EventHashTable::HashString("window_resize"));
            RK_CORE_ASSERT(ret, "Register window_resize Failed");
            ret = g_EventManager->AddListener(REGISTER_DELEGATE_CLASS(WindowManager::OnWindowResize, *g_WindowManager), EventHashTable::HashString("window_resize"));
            RK_CORE_ASSERT(ret, "Register window_resize Failed");
            ret = g_EventManager->AddListener(REGISTER_DELEGATE_CLASS(GameLogic::OnUIEvent, *g_GameLogic), EventHashTable::HashString("ui_event_logic"));
            RK_CORE_ASSERT(ret, "Register ui_event_logic Failed");
        }

        // Add Robot UI
        {
            auto ui = g_GraphicsManager->GetUI();
            ui->AddContext(CreateRef<RobotUI>());
        }

        // Setup Game Logic
        {
            Eigen::Matrix<uint64_t, 6, 6> stateTransferMatrix = Eigen::Matrix<uint64_t, 6, 6>::Zero();
            stateTransferMatrix(0, 0) = 0;
            Ref<StateMachine> stateMachine = CreateRef<StateMachine>("Robot Control");

            Ref<StateNode> init = CreateRef<StateNode>("init"); init->transferFun = init_2_other;

            Ref<StateNode> rot_00 = CreateRef<StateNode>("rot_00"); rot_00->transferFun = rot_00_2_other;
            Ref<StateNode> rot_01 = CreateRef<StateNode>("rot_01"); rot_01->transferFun = rot_01_2_other;
            Ref<StateNode> rot_02 = CreateRef<StateNode>("rot_02"); rot_02->transferFun = rot_02_2_other;

            Ref<StateNode> move_00 = CreateRef<StateNode>("move_00"); move_00->transferFun = move_00_2_other;
            Ref<StateNode> move_01 = CreateRef<StateNode>("move_01"); move_01->transferFun = move_01_2_other;
            Ref<StateNode> move_02 = CreateRef<StateNode>("move_02"); move_02->transferFun = move_02_2_other;
            Ref<StateNode> move_03 = CreateRef<StateNode>("move_03"); move_03->transferFun = move_03_2_other;
            Ref<StateNode> move_04 = CreateRef<StateNode>("move_04"); move_04->transferFun = move_04_2_other;
            Ref<StateNode> move_05 = CreateRef<StateNode>("move_05"); move_05->transferFun = move_05_2_other;

            // Set init
            {
                Ref<StateEdge> egde_02 = CreateRef<StateEdge>("egde_02");
                egde_02->parent = init;
                egde_02->child = rot_00;
                egde_02->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> egde_03 = CreateRef<StateEdge>("egde_03");
                egde_03->parent = init;
                egde_03->child = move_00;
                egde_03->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                init->AddEgde(egde_02);
                init->AddEgde(egde_03);
            }
            
            // Set rot_00
            {
                Ref<StateEdge> egde_20 = CreateRef<StateEdge>("egde_20");
                egde_20->parent = rot_00;
                egde_20->child = init;
                egde_20->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> egde_21 = CreateRef<StateEdge>("egde_21");
                egde_21->parent = rot_00;
                egde_21->child = rot_01;
                egde_21->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                rot_00->AddEgde(egde_20);
                rot_00->AddEgde(egde_21);
            }

            // Set rot_01
            {
                Ref<StateEdge> egde_22 = CreateRef<StateEdge>("egde_22");
                egde_22->parent = rot_01;
                egde_22->child = rot_02;
                egde_22->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                rot_01->AddEgde(egde_22);
            }
            
            // Set rot_02
            {
                Ref<StateEdge> egde_23 = CreateRef<StateEdge>("egde_23");
                egde_23->parent = rot_02;
                egde_23->child = rot_00;
                egde_23->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                rot_02->AddEgde(egde_23);
            }

            // Set move_00
            {
                Ref<StateEdge> egde_30 = CreateRef<StateEdge>("egde_30");
                egde_30->parent = move_00;
                egde_30->child = init;
                egde_30->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> egde_31 = CreateRef<StateEdge>("egde_31");
                egde_31->parent = move_00;
                egde_31->child = move_01;
                egde_31->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_00->AddEgde(egde_30);
                move_00->AddEgde(egde_31);
            }

            // Set move_01
            {
                Ref<StateEdge> egde_32 = CreateRef<StateEdge>("egde_32");
                egde_32->parent = move_01;
                egde_32->child = move_02;
                egde_32->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_01->AddEgde(egde_32);
            }

            // Set move_02
            {
                Ref<StateEdge> egde_33 = CreateRef<StateEdge>("egde_33");
                egde_33->parent = move_02;
                egde_33->child = move_03;
                egde_33->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_02->AddEgde(egde_33);
            }
            
            // Set move_03
            {
                Ref<StateEdge> egde_34 = CreateRef<StateEdge>("egde_34");
                egde_34->parent = move_03;
                egde_34->child = move_04;
                egde_34->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_03->AddEgde(egde_34);
            }

            // Set move_04
            {
                Ref<StateEdge> egde_35 = CreateRef<StateEdge>("egde_35");
                egde_35->parent = move_04;
                egde_35->child = move_05;
                egde_35->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_04->AddEgde(egde_35);
            }

            // Set move_05
            {
                Ref<StateEdge> egde_36 = CreateRef<StateEdge>("egde_36");
                egde_36->parent = move_05;
                egde_36->child = move_00;
                egde_36->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_05->AddEgde(egde_36);
            }

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

        if(1)
        {
            cam->SetProjectionType(SceneCamera::ProjectionType::Orthographic);
            auto width = g_WindowManager->GetWindowWidth();
            auto height = g_WindowManager->GetWindowHeight();
            cam->SetViewportSize(width, height);
            cam->SetOrthographic(10.0f, -1.0f, 10.0f);
        }
        else
        {
            cam->SetProjectionType(SceneCamera::ProjectionType::Perspective);
            auto width = g_WindowManager->GetWindowWidth();
            auto height = g_WindowManager->GetWindowHeight();
            cam->SetViewportSize(width, height);
            cam->SetPerspective(90.0f, 0.1f, 100.0f);
        }

        //std::default_random_engine e;
        //std::uniform_int_distribution<unsigned> u(0, 254);
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