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

using namespace Rocket;

SerialPortModule* g_SerialPort;

Application* Rocket::g_Application;
MemoryManager* Rocket::g_MemoryManager;
AssetLoader* Rocket::g_AssetLoader;
AudioManager* Rocket::g_AudioManager;
WindowManager* Rocket::g_WindowManager;
PipelineStateManager* Rocket::g_PipelineStateManager;
GraphicsManager* Rocket::g_GraphicsManager;
ProcessManager* Rocket::g_ProcessManager;
EventManager* Rocket::g_EventManager;
SceneManager* Rocket::g_SceneManager;
GameLogic* Rocket::g_GameLogic;

Application* Rocket::CreateApplication()
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

    g_SerialPort = new SerialPortModule;

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

    PushModule(g_SerialPort);
}

void SimpleApp::PostInitializeModule()
{
    // Register Event Listener
    {
        REGISTER_DELEGATE_CLASS(Application, Application::OnWindowClose, g_Application, window_close);
        REGISTER_DELEGATE_CLASS(SerialPortModule, SerialPortModule::OnWindowClose, g_SerialPort, window_close);
        REGISTER_DELEGATE_CLASS(GraphicsManager, GraphicsManager::OnWindowResize, g_GraphicsManager, window_resize);
        REGISTER_DELEGATE_CLASS(WindowManager, WindowManager::OnWindowResize, g_WindowManager, window_resize);
        REGISTER_DELEGATE_CLASS(GameLogic, GameLogic::OnUIEvent, g_GameLogic, ui_event_logic);
        REGISTER_DELEGATE_CLASS(GameLogic, GameLogic::OnResponseEvent, g_GameLogic, ui_event_response);
    }

    // Add Robot UI
    {
        auto ui = g_GraphicsManager->GetUI();
        ui->AddContext(CreateRef<RobotUI>());
    }

    // Setup Game Logic
    {
        // 0
        Ref<StateNode> init = CreateRef<StateNode>("init"); init->transferFun = update_along_mat;
        
        // 1
        Ref<StateNode> rot_00 = CreateRef<StateNode>("rot_00"); rot_00->transferFun = update_along_mat;
        // 2
        Ref<StateNode> rot_01 = CreateRef<StateNode>("rot_01"); rot_01->transferFun = update_along_mat;
        // 3
        Ref<StateNode> rot_02 = CreateRef<StateNode>("rot_02"); rot_02->transferFun = update_along_mat;

        // 10
        Ref<StateNode> rot_rec_0 = CreateRef<StateNode>("rot_rec_0"); rot_rec_0->transferFun = update_along_mat;
        // 11
        Ref<StateNode> rot_rec_1 = CreateRef<StateNode>("rot_rec_1"); rot_rec_1->transferFun = update_along_mat;

        // 4 in up
        Ref<StateNode> move_00 = CreateRef<StateNode>("move_00"); move_00->transferFun = update_along_mat;
        // 5 in forward
        Ref<StateNode> move_01 = CreateRef<StateNode>("move_01"); move_01->transferFun = update_along_mat;
        // 6 in down
        Ref<StateNode> move_02 = CreateRef<StateNode>("move_02"); move_02->transferFun = update_along_mat;
        // 7 out up
        Ref<StateNode> move_03 = CreateRef<StateNode>("move_03"); move_03->transferFun = update_along_mat;
        // 8 out forward
        Ref<StateNode> move_04 = CreateRef<StateNode>("move_04"); move_04->transferFun = update_along_mat;
        // 9 out down
        Ref<StateNode> move_05 = CreateRef<StateNode>("move_05"); move_05->transferFun = update_along_mat;
        
        // 12
        Ref<StateNode> move_out_up = CreateRef<StateNode>("move_out_up"); move_out_up->transferFun = update_along_mat;
        // 13
        Ref<StateNode> move_out_mid = CreateRef<StateNode>("move_out_mid"); move_out_mid->transferFun = update_along_mat;
        // 14
        Ref<StateNode> move_out_down = CreateRef<StateNode>("move_out_down"); move_out_down->transferFun = update_along_mat;

        // 15
        Ref<StateNode> move_in_up = CreateRef<StateNode>("move_in_up"); move_in_up->transferFun = update_along_mat;
        // 16
        Ref<StateNode> move_in_mid = CreateRef<StateNode>("move_in_mid"); move_in_mid->transferFun = update_along_mat;
        // 17
        Ref<StateNode> move_in_down = CreateRef<StateNode>("move_in_down"); move_in_down->transferFun = update_along_mat;

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

            Ref<StateEdge> edge_4_13 = CreateRef<StateEdge>("edge_4_13");
            edge_4_13->parent = move_00;
            edge_4_13->child = move_out_mid;
            edge_4_13->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_00->AddEgde(edge_40);
            move_00->AddEgde(edge_45);
            move_00->AddEgde(edge_4_13);
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

            Ref<StateEdge> edge_5_13 = CreateRef<StateEdge>("edge_5_13");
            edge_5_13->parent = move_01;
            edge_5_13->child = move_out_mid;
            edge_5_13->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_01->AddEgde(edge_50);
            move_01->AddEgde(edge_56);
            move_01->AddEgde(edge_5_13);
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

            Ref<StateEdge> edge_6_12 = CreateRef<StateEdge>("edge_6_12");
            edge_6_12->parent = move_02;
            edge_6_12->child = move_out_up;
            edge_6_12->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_02->AddEgde(edge_60);
            move_02->AddEgde(edge_67);
            move_02->AddEgde(edge_6_12);
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

            Ref<StateEdge> edge_7_16 = CreateRef<StateEdge>("edge_7_16");
            edge_7_16->parent = move_03;
            edge_7_16->child = move_in_mid;
            edge_7_16->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_03->AddEgde(edge_70);
            move_03->AddEgde(edge_78);
            move_03->AddEgde(edge_7_16);
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

            Ref<StateEdge> edge_8_16 = CreateRef<StateEdge>("edge_8_16");
            edge_8_16->parent = move_04;
            edge_8_16->child = move_in_mid;
            edge_8_16->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_04->AddEgde(edge_80);
            move_04->AddEgde(edge_89);
            move_04->AddEgde(edge_8_16);
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

            Ref<StateEdge> edge_9_15 = CreateRef<StateEdge>("edge_9_15");
            edge_9_15->parent = move_05;
            edge_9_15->child = move_in_up;
            edge_9_15->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_05->AddEgde(edge_90);
            move_05->AddEgde(edge_94);
            move_05->AddEgde(edge_9_15);
        }

        // Set move rec
        {
            Ref<StateEdge> edge_15_16 = CreateRef<StateEdge>("edge_15_16");
            edge_15_16->parent = move_in_up;
            edge_15_16->child = move_in_mid;
            edge_15_16->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_in_up->AddEgde(edge_15_16);

            Ref<StateEdge> edge_16_17 = CreateRef<StateEdge>("edge_16_17");
            edge_16_17->parent = move_in_mid;
            edge_16_17->child = move_in_down;
            edge_16_17->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_in_mid->AddEgde(edge_16_17);

            Ref<StateEdge> edge_17_0 = CreateRef<StateEdge>("edge_17_0");
            edge_17_0->parent = move_in_down;
            edge_17_0->child = init;
            edge_17_0->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_in_down->AddEgde(edge_17_0);

            Ref<StateEdge> edge_12_13 = CreateRef<StateEdge>("edge_12_13");
            edge_12_13->parent = move_out_up;
            edge_12_13->child = move_out_mid;
            edge_12_13->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_out_up->AddEgde(edge_12_13);

            Ref<StateEdge> edge_13_14 = CreateRef<StateEdge>("edge_13_14");
            edge_13_14->parent = move_out_mid;
            edge_13_14->child = move_out_down;
            edge_13_14->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_out_mid->AddEgde(edge_13_14);

            Ref<StateEdge> edge_14_0 = CreateRef<StateEdge>("edge_14_0");
            edge_14_0->parent = move_out_down;
            edge_14_0->child = init;
            edge_14_0->actionFun = [](const Vec<Variant>&, const Vec<Variant>&){ return true; };

            move_out_down->AddEgde(edge_14_0);
        }

        initialize_variable();
        Ref<StateMachine> stateMachine = CreateRef<StateMachine>("Robot Control");
        stateMachine->SetInitState(init);
        g_GameLogic->SetStateMachine(stateMachine);
    }

    // Begin Serial Thread
    {
        m_ThreadPool.enqueue_work([](){
            g_SerialPort->MainLoop();
        });
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
