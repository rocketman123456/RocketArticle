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
            ret = g_EventManager->AddListener(REGISTER_DELEGATE_CLASS(Application::OnWindowClose, *g_Application), GlobalHashTable::HashString("Event"_hash, "window_close"));
            RK_CORE_ASSERT(ret, "Register window_close Failed");
            ret = g_EventManager->AddListener(REGISTER_DELEGATE_CLASS(GraphicsManager::OnWindowResize, *g_GraphicsManager), GlobalHashTable::HashString("Event"_hash, "window_resize"));
            RK_CORE_ASSERT(ret, "Register window_resize Failed");
            ret = g_EventManager->AddListener(REGISTER_DELEGATE_CLASS(WindowManager::OnWindowResize, *g_WindowManager), GlobalHashTable::HashString("Event"_hash, "window_resize"));
            RK_CORE_ASSERT(ret, "Register window_resize Failed");

            ret = g_EventManager->AddListener(REGISTER_DELEGATE_CLASS(GameLogic::OnUIEvent, *g_GameLogic), GlobalHashTable::HashString("Event"_hash, "ui_event_logic"));
            RK_CORE_ASSERT(ret, "Register ui_event_logic Failed");
            ret = g_EventManager->AddListener(REGISTER_DELEGATE_CLASS(GameLogic::OnResponseEvent, *g_GameLogic), GlobalHashTable::HashString("Event"_hash, "ui_event_response"));
            RK_CORE_ASSERT(ret, "Register ui_event_response Failed");
        }

        // Add Robot UI
        {
            auto ui = g_GraphicsManager->GetUI();
            ui->AddContext(CreateRef<RobotUI>());
        }

        // Setup Game Logic
        {
            Ref<StateNode> init = CreateRef<StateNode>("init"); init->transferFun = update_along_mat;

            Ref<StateNode> rot_00 = CreateRef<StateNode>("rot_00"); rot_00->transferFun = update_along_mat;
            Ref<StateNode> rot_01 = CreateRef<StateNode>("rot_01"); rot_01->transferFun = update_along_mat;
            Ref<StateNode> rot_02 = CreateRef<StateNode>("rot_02"); rot_02->transferFun = update_along_mat;

            Ref<StateNode> rot_rec_0 = CreateRef<StateNode>("rot_rec_0"); rot_rec_0->transferFun = update_along_mat;
            Ref<StateNode> rot_rec_1 = CreateRef<StateNode>("rot_rec_1"); rot_rec_1->transferFun = update_along_mat;

            Ref<StateNode> move_00 = CreateRef<StateNode>("move_00"); move_00->transferFun = update_along_mat;
            Ref<StateNode> move_01 = CreateRef<StateNode>("move_01"); move_01->transferFun = update_along_mat;
            Ref<StateNode> move_02 = CreateRef<StateNode>("move_02"); move_02->transferFun = update_along_mat;
            Ref<StateNode> move_03 = CreateRef<StateNode>("move_03"); move_03->transferFun = update_along_mat;
            Ref<StateNode> move_04 = CreateRef<StateNode>("move_04"); move_04->transferFun = update_along_mat;
            Ref<StateNode> move_05 = CreateRef<StateNode>("move_05"); move_05->transferFun = update_along_mat;

            // Set init 0
            {
                Ref<StateEdge> edge_01 = CreateRef<StateEdge>("edge_01");
                edge_01->parent = init;
                edge_01->child = rot_00;
                edge_01->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> edge_04 = CreateRef<StateEdge>("edge_04");
                edge_04->parent = init;
                edge_04->child = move_00;
                edge_04->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                init->AddEgde(edge_01);
                init->AddEgde(edge_04);
            }
            
            // Set rot_00 1
            {
                Ref<StateEdge> edge_10 = CreateRef<StateEdge>("edge_10");
                edge_10->parent = rot_00;
                edge_10->child = init;
                edge_10->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> edge_12 = CreateRef<StateEdge>("edge_12");
                edge_12->parent = rot_00;
                edge_12->child = rot_01;
                edge_12->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                rot_00->AddEgde(edge_10);
                rot_00->AddEgde(edge_12);
            }

            // Set rot_rec
            {
                Ref<StateEdge> edge_10_11 = CreateRef<StateEdge>("edge_10_11");
                edge_10_11->parent = rot_rec_0;
                edge_10_11->child = rot_rec_1;
                edge_10_11->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> edge_11_1 = CreateRef<StateEdge>("edge_11_1");
                edge_11_1->parent = rot_rec_1;
                edge_11_1->child = rot_00;
                edge_11_1->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                rot_rec_0->AddEgde(edge_10_11);
                rot_rec_1->AddEgde(edge_11_1);
            }

            // Set rot_01 2
            {
                Ref<StateEdge> edge_23 = CreateRef<StateEdge>("edge_23");
                edge_23->parent = rot_01;
                edge_23->child = rot_02;
                edge_23->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                rot_01->AddEgde(edge_23);
            }
            
            // Set rot_02 3
            {
                Ref<StateEdge> edge_32 = CreateRef<StateEdge>("edge_32");
                edge_32->parent = rot_02;
                edge_32->child = rot_01;
                edge_32->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> edge_3_10 = CreateRef<StateEdge>("edge_3_10");
                edge_3_10->parent = rot_02;
                edge_3_10->child = rot_rec_0;
                edge_3_10->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                rot_02->AddEgde(edge_32);
                rot_02->AddEgde(edge_3_10);
            }

            // Set move_00 4
            {
                Ref<StateEdge> edge_40 = CreateRef<StateEdge>("edge_40");
                edge_40->parent = move_00;
                edge_40->child = init;
                edge_40->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> edge_45 = CreateRef<StateEdge>("edge_45");
                edge_45->parent = move_00;
                edge_45->child = move_01;
                edge_45->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_00->AddEgde(edge_40);
                move_00->AddEgde(edge_45);
            }

            // Set move_01 5
            {
                Ref<StateEdge> edge_50 = CreateRef<StateEdge>("edge_50");
                edge_50->parent = move_01;
                edge_50->child = init;
                edge_50->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> edge_56 = CreateRef<StateEdge>("edge_56");
                edge_56->parent = move_01;
                edge_56->child = move_02;
                edge_56->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_01->AddEgde(edge_50);
                move_01->AddEgde(edge_56);
            }

            // Set move_02 6
            {
                Ref<StateEdge> edge_60 = CreateRef<StateEdge>("edge_60");
                edge_60->parent = move_02;
                edge_60->child = init;
                edge_60->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> edge_67 = CreateRef<StateEdge>("edge_67");
                edge_67->parent = move_02;
                edge_67->child = move_03;
                edge_67->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_02->AddEgde(edge_60);
                move_02->AddEgde(edge_67);
            }
            
            // Set move_03 7
            {
                Ref<StateEdge> edge_70 = CreateRef<StateEdge>("edge_70");
                edge_70->parent = move_03;
                edge_70->child = init;
                edge_70->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> edge_78 = CreateRef<StateEdge>("edge_78");
                edge_78->parent = move_03;
                edge_78->child = move_04;
                edge_78->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_03->AddEgde(edge_70);
                move_03->AddEgde(edge_78);
            }

            // Set move_04 8
            {
                Ref<StateEdge> edge_80 = CreateRef<StateEdge>("edge_80");
                edge_80->parent = move_04;
                edge_80->child = init;
                edge_80->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> edge_89 = CreateRef<StateEdge>("edge_89");
                edge_89->parent = move_04;
                edge_89->child = move_05;
                edge_89->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_04->AddEgde(edge_80);
                move_04->AddEgde(edge_89);
            }

            // Set move_05 9
            {
                Ref<StateEdge> edge_90 = CreateRef<StateEdge>("edge_90");
                edge_90->parent = move_05;
                edge_90->child = init;
                edge_90->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                Ref<StateEdge> edge_94 = CreateRef<StateEdge>("edge_94");
                edge_94->parent = move_05;
                edge_94->child = move_00;
                edge_94->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

                move_05->AddEgde(edge_90);
                move_05->AddEgde(edge_94);
            }

            initialize_variable();
            Ref<StateMachine> stateMachine = CreateRef<StateMachine>("Robot Control");
            stateMachine->SetInitState(init);
            g_GameLogic->SetStateMachine(stateMachine);
        }

        // hash test
        {
            auto hash_result = "hash"_hash;
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