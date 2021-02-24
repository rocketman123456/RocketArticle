#include "OpenGL/OpenGLGraphicsManager.h"
#include "Module/WindowManager.h"
#include "Module/Application.h"
#include "Module/MemoryManager.h"

#include "Scene/Scene.h"
#include "Scene/Component/PlanarMesh.h"

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

static void CheckForGLErrors()
{
    GLenum error = glGetError();
    switch (error) {
    case GL_NO_ERROR:
        return;
        break;
    case GL_INVALID_ENUM:
        RK_GRAPHICS_ERROR("Invalid enum!");
        break;
    case GL_INVALID_VALUE:
        RK_GRAPHICS_ERROR("Invalid value!");
        break;
    case GL_INVALID_OPERATION:
        RK_GRAPHICS_ERROR("Invalid operation!");
        break;
    case GL_STACK_OVERFLOW:
        RK_GRAPHICS_ERROR("Stack overflow!");
        break;
    case GL_STACK_UNDERFLOW:
        RK_GRAPHICS_ERROR("Stack underflow!");
        break;
    case GL_OUT_OF_MEMORY:
        RK_GRAPHICS_ERROR("Out Of memory!");
        break;
    default:
        RK_GRAPHICS_ERROR("Unknown error!");
        break;
    }
}

GraphicsManager* Rocket::GetGraphicsManager() { return new OpenGLGraphicsManager(); }

int OpenGLGraphicsManager::Initialize()
{
    //PROFILE_BIND_OPENGL();

    int ret = GraphicsManager::Initialize();
    if(ret != 0)
        return ret;

    m_WindowHandle = static_cast<GLFWwindow*>(g_WindowManager->GetNativeWindow());

    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    RK_CORE_ASSERT(status, "Failed to Initialize GLAD");

    RK_GRAPHICS_INFO("OpenGL Info:");
    RK_GRAPHICS_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    RK_GRAPHICS_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
    RK_GRAPHICS_INFO("  Version: {0}", glGetString(GL_VERSION));

    auto config = g_Application->GetConfig();
    m_VSync = config->GetConfigInfo<int8_t>("Graphics", "vsync");

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

    // Init UBOs
    for(size_t i = 0; i < m_MaxFrameInFlight; ++i)
    {
        m_uboDrawFrameConstant[i] = CreateRef<OpenGLUniformBuffer>(sizeof(PerFrameConstants), DRAW_TYPE::STATIC);
        m_uboDrawBatchConstant[i] = CreateRef<OpenGLUniformBuffer>(sizeof(PerBatchConstants), DRAW_TYPE::STATIC);
        m_uboLightInfo[i] = CreateRef<OpenGLUniformBuffer>(sizeof(LightInfo), DRAW_TYPE::STATIC);;
        m_uboShadowMatricesConstant[i] = nullptr;
    }

    // ImGui Init
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
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
    io.Fonts->AddFontDefault();

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

void OpenGLGraphicsManager::BeginScene(const Scene& scene)
{
    GraphicsManager::BeginScene(scene);

    if (!m_CurrentScene)
    {
        RK_GRAPHICS_INFO("No Active Scene");
        return;
    }

    auto planar_meshes = m_CurrentScene->GetComponents<PlanarMesh>();
    for (auto mesh : planar_meshes)
    {
        auto dbc = CreateRef<OpenGLDrawBatchContext>();
        auto vertex = mesh->GetVertex();
        auto index = mesh->GetIndex();
        dbc->VAO = CreateRef<OpenGLVertexArray>();
        //RK_GRAPHICS_INFO("Size of QuadVertex {}", sizeof(QuadVertex));
        auto vbo = CreateRef<OpenGLVertexBuffer>(vertex.size() * sizeof(QuadVertex));
        vbo->SetLayout({
            { ShaderDataType::Vec4f, "a_Position" },
            { ShaderDataType::Vec4f, "a_Color" },
            { ShaderDataType::Vec2f, "a_TexCoord" },
            { ShaderDataType::Float, "a_TexIndex" },
            { ShaderDataType::Float, "a_TilingFactor" },
        });
        vbo->SetData(vertex.data(), vertex.size() * sizeof(QuadVertex));
        auto ibo = CreateRef<OpenGLIndexBuffer>(index.data(), index.size());
        dbc->VAO->AddVertexBuffer(vbo);
        dbc->VAO->SetIndexBuffer(ibo);
        dbc->Count = ibo->GetCount();
        dbc->Textures = &(mesh->GetTexture());
        dbc->MaxTextures = mesh->GetTextureCount();

        // TODO : use real model matrix
        dbc->modelMatrix = Matrix4f::Identity();
        // TODO : set draw element type and mode
        //  mode = GL_POINTS;
        //  mode = GL_LINES;
        //  mode = GL_LINE_STRIP;
        //  mode = GL_TRIANGLES;
        //  mode = GL_TRIANGLE_STRIP;
        //  mode = GL_TRIANGLE_FAN;
        dbc->Mode = GL_TRIANGLES;
        //  type = GL_UNSIGNED_BYTE;
        //  type = GL_UNSIGNED_SHORT;
        //  type = GL_UNSIGNED_INT;
        dbc->Type = GL_UNSIGNED_INT;

        for (int32_t n = 0; n < m_MaxFrameInFlight; n++)
        {
            m_Frames[n].batchContexts.push_back(dbc);
        }
    }
}

void OpenGLGraphicsManager::EndScene()
{
    GraphicsManager::EndScene();
}

void OpenGLGraphicsManager::BeginFrame(const Frame& frame)
{
    // Set the color to clear the screen to.
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    // Clear the screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetPerFrameConstants(frame.frameContext);
    SetLightInfo(frame.frameContext);

    // TODO : move this function into standard pipeline
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void OpenGLGraphicsManager::SetPerFrameConstants(const DrawFrameContext& context)
{
    auto constant = static_cast<PerFrameConstants>(context);
    m_uboDrawFrameConstant[m_CurrentFrameIndex]->SetSubData(&constant, 0, sizeof(PerFrameConstants));
}

void OpenGLGraphicsManager::SetLightInfo(const DrawFrameContext& context)
{
    //auto constant = static_cast<PerFrameConstants>(context.lightInfo);
    //m_uboLightInfo[m_CurrentFrameIndex]->SetSubData(&constant, 0, sizeof(LightInfo));
}

void OpenGLGraphicsManager::SetPipelineState(const Ref<PipelineState> &pipelineState, const Frame &frame)
{
    m_CurrentPipelineState = pipelineState;
    const OpenGLPipelineState* pPipelineState = dynamic_cast<const OpenGLPipelineState*>(m_CurrentPipelineState.get());
    m_CurrentShader = pPipelineState->shaderProgram;

    // Set Frame Buffer
    if (pipelineState->renderTarget == RENDER_TARGET::NONE)
    {
        m_CurrentFrameBuffer = nullptr;
    }
    else
    {
        auto it = m_FrameBuffers.find(pipelineState->renderTargetName);
        if (it == m_FrameBuffers.end())
        {
            m_CurrentFrameBuffer = CreateRef<OpenGLFrameBuffer>(pipelineState->frameBufferInfo);
            m_FrameBuffers[pipelineState->renderTargetName] = m_CurrentFrameBuffer;
        }
        else
        {
            m_CurrentFrameBuffer = it->second;
        }
    }

    // Bind Framebuffer
    if (m_CurrentFrameBuffer)
    {
        m_CurrentFrameBuffer->Bind(FRAME_BIND_MODE::FRAMEBUFFER);
    }

    // Set OpenGL State
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

    if (pipelineState->depthWriteMode)
        glDepthMask(GL_TRUE);
    else
        glDepthMask(GL_FALSE);

    // Set Blender Mode
    switch (pipelineState->blenderMode)
    {
    case BLENDER_MODE::NONE:
        glDisable(GL_BLEND);
        break;
    case BLENDER_MODE::ONE_MINUS_SRC_ALPHA:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case BLENDER_MODE::ONE_MINUS_DST_ALPHA:
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
        break;
    }

    // Set Cullface Mode
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
}

void OpenGLGraphicsManager::BeginFrameBuffer(const Frame& frame)
{
    if (m_CurrentFrameBuffer)
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Set the color shader
    m_CurrentShader->Bind();

    // Bind PerFrameConstants
    uint32_t shader_id = m_CurrentShader->GetRenderId();
    uint32_t frame_block_index = glGetUniformBlockIndex(shader_id, "PerFrameConstants");
    if (frame_block_index != GL_INVALID_INDEX)
    {
        int32_t blockSize;
        glGetActiveUniformBlockiv(shader_id, frame_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
        RK_GRAPHICS_ASSERT(blockSize >= sizeof(PerFrameConstants), "PerFrameConstants Size Error");
        m_uboDrawFrameConstant[m_CurrentFrameIndex]->BindShader(shader_id, frame_block_index, 0);
    }

    // Bind PerBatchConstants
    shader_id = m_CurrentShader->GetRenderId();
    frame_block_index = glGetUniformBlockIndex(shader_id, "PerBatchConstants");
    if (frame_block_index != GL_INVALID_INDEX)
    {
        int32_t blockSize;
        glGetActiveUniformBlockiv(shader_id, frame_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
        RK_GRAPHICS_ASSERT(blockSize >= sizeof(PerBatchConstants), "PerBatchConstants Size Error");
        m_uboDrawBatchConstant[m_CurrentFrameIndex]->BindShader(shader_id, frame_block_index, 1);
    }

    // TODO : remove fixed 16, use graphics card capability
    int32_t samplers[16];
    for (uint32_t i = 0; i < 16; i++)
        samplers[i] = i;
    m_CurrentShader->SetInt32Array("u_Textures", samplers, 16);
}

void OpenGLGraphicsManager::SetPerBatchConstants(const DrawBatchContext &context)
{
    auto constant = static_cast<PerBatchConstants>(context);
    m_uboDrawBatchConstant[m_CurrentFrameIndex]->SetSubData(&constant, 0, sizeof(PerBatchConstants));
}

void OpenGLGraphicsManager::DrawBatch(const Frame &frame)
{
    BeginFrameBuffer(frame);
    for(auto& pDbc : frame.batchContexts)
    {
        SetPerBatchConstants(*pDbc);
        const auto& dbc = dynamic_cast<const OpenGLDrawBatchContext&>(*pDbc);

        dbc.VAO->Bind();

        for(int i = 0; i < dbc.MaxTextures; ++i)
        {
            (*dbc.Textures)[i]->Bind(i);
        }

        glDrawElements(dbc.Mode, dbc.Count, dbc.Type, nullptr);
        //glBindTexture(GL_TEXTURE_2D, 0);
    }
    EndFrameBuffer(frame);
}

void OpenGLGraphicsManager::EndFrameBuffer(const Frame& frame)
{
    if (m_CurrentFrameBuffer)
    {
        m_CurrentFrameBuffer->Unbind();
    }
}

void OpenGLGraphicsManager::EndFrame(const Frame& frame)
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
    if (e->GetInt32(3) == 0)
    {
        m_Width = e->GetInt32(1);
        m_Height = e->GetInt32(2);
        return Resize(m_Width, m_Height);
    }
    else
    {
        return false;
    }
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

//------------------------------------------------------------
//-- Debug ---------------------------------------------------
//------------------------------------------------------------

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
