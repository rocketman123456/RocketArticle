#include "OpenGL/OpenGLGraphicsManager.h"
#include "OpenGL/OpenGLPipelineStateManager.h"
#include "Module/WindowManager.h"
#include "Module/Application.h"
#include "Module/MemoryManager.h"
//#include "Scene/Component.h"
//#include "Scene/Entity.h"
#include "Scene/Scene.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_opengl3.cpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_glfw.cpp"

using namespace Rocket;

static void OpenGLMessageCallback(unsigned source, unsigned type, unsigned id,
                                  unsigned severity, int length,
                                  const char *message, const void *userParam)
{
    RK_GRAPHICS_INFO("---------------");
    RK_GRAPHICS_INFO("Debug message ({0})", id);

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        RK_GRAPHICS_INFO("Source: API");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        RK_GRAPHICS_INFO("Source: Window System");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        RK_GRAPHICS_INFO("Source: Shader Compiler");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        RK_GRAPHICS_INFO("Source: Third Party");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        RK_GRAPHICS_INFO("Source: Application");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        RK_GRAPHICS_INFO("Source: Other");
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        RK_GRAPHICS_INFO("Type: Error");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        RK_GRAPHICS_INFO("Type: Deprecated Behaviour");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        RK_GRAPHICS_INFO("Type: Undefined Behaviour");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        RK_GRAPHICS_INFO("Type: Portability");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        RK_GRAPHICS_INFO("Type: Performance");
        break;
    case GL_DEBUG_TYPE_MARKER:
        RK_GRAPHICS_INFO("Type: Marker");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        RK_GRAPHICS_INFO("Type: Push Group");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        RK_GRAPHICS_INFO("Type: Pop Group");
        break;
    case GL_DEBUG_TYPE_OTHER:
        RK_GRAPHICS_INFO("Type: Other");
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        RK_GRAPHICS_CRITICAL(message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        RK_GRAPHICS_ERROR(message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        RK_GRAPHICS_WARN(message);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        RK_GRAPHICS_INFO(message);
        break;
    default:
        RK_CORE_ASSERT(false, "Unknown severity level!");
        break;
    }
}

GraphicsManager * Rocket::GetGraphicsManager() { return new OpenGLGraphicsManager(); }

int OpenGLGraphicsManager::Initialize()
{
    //PROFILE_BIND_OPENGL();

    int ret = GraphicsManager::Initialize();
    if(ret != 0)
        return ret;

    m_WindowHandle = static_cast<GLFWwindow *>(g_WindowManager->GetNativeWindow());

    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    RK_CORE_ASSERT(status, "Failed to Initialize GLAD");

    RK_GRAPHICS_INFO("OpenGL Info:");
    RK_GRAPHICS_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    RK_GRAPHICS_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
    RK_GRAPHICS_INFO("  Version: {0}", glGetString(GL_VERSION));

    auto config = g_Application->GetConfig();

    if (m_VSync)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
    }

    // ImGui Init
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows 
    // can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Load Fonts
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
        
    ImGui_ImplGlfw_InitForOpenGL(m_WindowHandle, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    return 0;
}

void OpenGLGraphicsManager::Finalize()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // Finalize
    GraphicsManager::Finalize();
    //PROFILE_UNBIND_OPENGL();
}

void OpenGLGraphicsManager::Present() { SwapBuffers(); }

void OpenGLGraphicsManager::Tick(Timestep ts)
{
    //PROFILE_SCOPE_OPENGL(OpenGLTick);
    //PROFILE_BEGIN_OPENGL(OpenGLRender);
    PROFILE_BEGIN_CPU_SAMPLE(OpenGLGraphicsManagerUpdate, 0);

    GraphicsManager::Tick(ts);

    PROFILE_END_CPU_SAMPLE();
    //PROFILE_END_OPENGL();
}

void OpenGLGraphicsManager::SetPipelineState(const Ref<PipelineState> &pipelineState, const Frame &frame)
{
    const Ref<const OpenGLPipelineState> pPipelineState =
        std::dynamic_pointer_cast<const OpenGLPipelineState>(pipelineState);
    m_CurrentShader = pPipelineState->shaderProgram;

    // Set the color shader as the current shader program and set the matrices
    // that it will use for rendering.
    m_CurrentShader->Bind();

    switch (pipelineState->depthTestMode)
    {
    case DEPTH_TEST_MODE::NONE:
        glDisable(GL_DEPTH_TEST);
        break;
    case DEPTH_TEST_MODE::LARGE:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GREATER);
        break;
    case DEPTH_TEST_MODE::LARGE_EQUAL:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GEQUAL);
        break;
    case DEPTH_TEST_MODE::LESS:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        break;
    case DEPTH_TEST_MODE::LESS_EQUAL:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        break;
    case DEPTH_TEST_MODE::EQUAL:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_EQUAL);
        break;
    case DEPTH_TEST_MODE::NOT_EQUAL:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_NOTEQUAL);
        break;
    case DEPTH_TEST_MODE::NEVER:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_NEVER);
        break;
    case DEPTH_TEST_MODE::ALWAYS:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
        break;
    default:
        assert(0);
    }

    if (pipelineState->bDepthWrite)
        glDepthMask(GL_TRUE);
    else
        glDepthMask(GL_FALSE);

    switch (pipelineState->cullFaceMode)
    {
    case CULL_FACE_MODE::NONE:
        glDisable(GL_CULL_FACE);
        break;
    case CULL_FACE_MODE::FRONT:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
    case CULL_FACE_MODE::BACK:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    default:
        assert(0);
    }
    
    if (pPipelineState->pipelineStateName == "Draw2D")
    {
    }
}

void OpenGLGraphicsManager::BeginFrame(const Frame &frame)
{
    // Set the color to clear the screen to.
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // Clear the screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetPerFrameConstants(frame.frameContext);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void OpenGLGraphicsManager::EndFrame(const Frame &frame)
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    GraphicsManager::EndFrame(frame);
}

void OpenGLGraphicsManager::BeginScene(const Scene& scene)
{ 
    GraphicsManager::BeginScene(scene);
}

void OpenGLGraphicsManager::EndScene()
{
    for (int i = 0; i < m_Frames.size(); i++)
    {
        auto &batchContexts = m_Frames[i].batchContexts;
        batchContexts.clear();
    }
    GraphicsManager::EndScene();
}

void OpenGLGraphicsManager::SetPerFrameConstants(const DrawFrameContext& context)
{
}

void OpenGLGraphicsManager::SetPerBatchConstants(const DrawBatchContext &context)
{
}

void OpenGLGraphicsManager::DrawBatch(const Frame &frame)
{
}

void OpenGLGraphicsManager::DrawFullScreenQuad()
{
    GLfloat vertices[] = {
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    GLfloat uv[] = {
        0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f
    };

    uint32_t vao;
    glGenVertexArrays(1, &vao);

    // Bind the vertex array object to store all the buffers and vertex attributes we create here.
    glBindVertexArray(vao);

    uint32_t buffer_id[2];

    // Generate an ID for the vertex buffer.
    glGenBuffers(2, buffer_id);

    // Bind the vertex buffer and load the vertex (position) data into the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);

    // Bind the vertex buffer and load the vertex (uv) data into the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);

    glDrawArrays(GL_TRIANGLE_STRIP, 0x00, 4);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, buffer_id);
}

void OpenGLGraphicsManager::SwapBuffers()
{
    glfwSwapBuffers(m_WindowHandle);
}

bool OpenGLGraphicsManager::Resize(int32_t width, int32_t height)
{
    glViewport(0, 0, width, height);
    return false;
}

bool OpenGLGraphicsManager::OnWindowResize(EventPtr& e)
{
    int32_t width = e->GetInt32(1);
    int32_t height = e->GetInt32(2);
    return Resize(width, height);
}

void OpenGLGraphicsManager::DrawPoint(const Point3D &point, const Vector3f &color)
{
}

void OpenGLGraphicsManager::DrawPointSet(const Point3DSet &point_set, const Vector3f &color)
{
}

void OpenGLGraphicsManager::DrawPointSet(const Point3DSet &point_set, const Matrix4f &trans, const Vector3f &color)
{
}

void OpenGLGraphicsManager::DrawLine(const Point3D &from, const Point3D &to, const Vector3f &color)
{
}

void OpenGLGraphicsManager::DrawLine(const Point3DList &vertices, const Vector3f &color)
{
}

void OpenGLGraphicsManager::DrawLine(const Point3DList &vertices, const Matrix4f &trans, const Vector3f &color)
{
}

void OpenGLGraphicsManager::DrawTriangle(const Point3DList &vertices, const Vector3f &color)
{
}

void OpenGLGraphicsManager::DrawTriangle(const Point3DList &vertices, const Matrix4f &trans, const Vector3f &color)
{
}

void OpenGLGraphicsManager::DrawTriangleStrip(const Point3DList &vertices, const Vector3f &color)
{
}
