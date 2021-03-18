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
#include "Logic/StateMachineSerializer.h"

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
        REGISTER_DELEGATE_CLASS(GraphicsManager, GraphicsManager::OnWindowResize, g_GraphicsManager, window_resize);
        REGISTER_DELEGATE_CLASS(WindowManager, WindowManager::OnWindowResize, g_WindowManager, window_resize);
        REGISTER_DELEGATE_CLASS(GameLogic, GameLogic::OnUIEvent, g_GameLogic, ui_event_logic);
        REGISTER_DELEGATE_CLASS(GameLogic, GameLogic::OnResponseEvent, g_GameLogic, ui_event_response);
        REGISTER_DELEGATE_CLASS(SerialPortModule, SerialPortModule::OnWindowClose, g_SerialPort, window_close);
        REGISTER_DELEGATE_CLASS(SerialPortModule, SerialPortModule::OnAction, g_SerialPort, ui_event_logic);
        REGISTER_DELEGATE_CLASS(SerialPortModule, SerialPortModule::OnMotor, g_SerialPort, ui_event_motor);
    }

    // Add Robot UI
    {
        auto ui = g_GraphicsManager->GetUI();
        auto ui_context = CreateRef<RobotUI>();
        ui->AddContext(ui_context);

        auto ui_raw_ptr = ui_context.get();
        REGISTER_DELEGATE_CLASS(RobotUI, RobotUI::OnResponseEvent, ui_raw_ptr, ui_event_response);
    }

    // Add State Machine
    {
        Ref<StateMachine> stateMachine = StateMachineSerializer::Deserialize("Logic/robot-state-machine.yaml");
        g_GameLogic->SetStateMachine(stateMachine);

        StateMachineSerializer::Serialize("Logic/robot-control.yaml", stateMachine);
    }

    // Begin Serial Thread
    {
        m_ThreadPool.enqueue_work([](){ g_SerialPort->MainLoop(); });
    }
}

void SimpleApp::PreInitialize()
{
    // Setup game asset
    Ref<Scene> scene = CreateRef<Scene>("Test Scene");
    Scope<SceneNode> root_node = CreateScope<SceneNode>("Root Node");
    Scope<SceneNode> cam_node = CreateScope<SceneNode>("Camera Node");
    //Scope<SceneNode> mesh_node = CreateScope<SceneNode>("Mesh Node");

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
    //root_node->AddChild(*mesh_node);

    scene->AddNode(std::move(root_node));
    //Vec<Scope<SceneComponent>> mesh_components;
    //mesh_components.push_back(std::move(mesh));
    //scene->SetComponents(mesh->GetType(), std::move(mesh_components));

    auto ret_1 = g_SceneManager->AddScene(scene);
    auto ret_2 = g_SceneManager->SetActiveScene("Test Scene");
}

void SimpleApp::PostInitialize()
{
}
