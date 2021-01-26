#include "OpenGL/OpenGLGraphicsManager.h"
#include "OpenGL/OpenGLPipelineStateManager.h"
#include "Module/WindowManager.h"
#include "Event/ApplicationEvent.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

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

namespace Rocket
{
    GraphicsManager *GetGraphicsManager() { return new OpenGLGraphicsManager(); }

    int OpenGLGraphicsManager::Initialize()
    {
        PROFILE_BIND_OPENGL();

        m_WindowHandle =
            static_cast<GLFWwindow *>(g_WindowManager->GetNativeWindow());

        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        RK_CORE_ASSERT(status, "Failed to Initialize GLAD");

        RK_GRAPHICS_INFO("OpenGL Info:");
        RK_GRAPHICS_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
        RK_GRAPHICS_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
        RK_GRAPHICS_INFO("  Version: {0}", glGetString(GL_VERSION));

        if (m_VSync)
        {
            glfwSwapInterval(1);
        }
        else
        {
            glfwSwapInterval(0);
        }

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(OpenGLMessageCallback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                                  GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL,
                                  GL_FALSE);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);

        return 0;
    }

    void OpenGLGraphicsManager::Finalize() { PROFILE_UNBIND_OPENGL(); }

    void OpenGLGraphicsManager::Present() { SwapBuffers(); }

    void OpenGLGraphicsManager::Tick(Timestep ts)
    {
        PROFILE_SCOPE_OPENGL(OpenGLTick);
        PROFILE_BEGIN_OPENGL(OpenGLRender);
        PROFILE_BEGIN_CPU_SAMPLE(OpenGLGraphicsManagerUpdate, 0);

        GraphicsManager::Tick(ts);

        PROFILE_END_CPU_SAMPLE();
        PROFILE_END_OPENGL();
    }

    void OpenGLGraphicsManager::SetPipelineState(const Ref<PipelineState> &pipelineState, const Frame &frame)
    {
        const Ref<const OpenGLPipelineState> pPipelineState =
            std::dynamic_pointer_cast<const OpenGLPipelineState>(pipelineState);
        m_CurrentShader = pPipelineState->shaderProgram;

        // Set the color shader as the current shader program and set the matrices
        // that it will use for rendering.
        glUseProgram(m_CurrentShader);

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

        // Prepare & Bind per frame constant buffer
        uint32_t blockIndex = glGetUniformBlockIndex(m_CurrentShader, "PerFrameConstants");

        if (blockIndex != GL_INVALID_INDEX)
        {
            int32_t blockSize;

            glGetActiveUniformBlockiv(m_CurrentShader, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

            assert(blockSize >= sizeof(PerFrameConstants));

            glUniformBlockBinding(m_CurrentShader, blockIndex, 10);
            glBindBufferBase(GL_UNIFORM_BUFFER, 10, m_uboDrawFrameConstant[frame.frameIndex]);
        }

        // Prepare per batch constant buffer binding point
        blockIndex = glGetUniformBlockIndex(m_CurrentShader, "PerBatchConstants");

        if (blockIndex != GL_INVALID_INDEX)
        {
            int32_t blockSize;

            glGetActiveUniformBlockiv(m_CurrentShader, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

            assert(blockSize >= sizeof(PerBatchConstants));

            glUniformBlockBinding(m_CurrentShader, blockIndex, 11);
            glBindBufferBase(GL_UNIFORM_BUFFER, 11, m_uboDrawBatchConstant[frame.frameIndex]);
        }

        // Prepare & Bind light info
        blockIndex = glGetUniformBlockIndex(m_CurrentShader, "LightInfo");

        if (blockIndex != GL_INVALID_INDEX)
        {
            int32_t blockSize;

            glGetActiveUniformBlockiv(m_CurrentShader, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

            assert(blockSize >= sizeof(LightInfo));

            glUniformBlockBinding(m_CurrentShader, blockIndex, 12);
            glBindBufferBase(GL_UNIFORM_BUFFER, 12, m_uboLightInfo[frame.frameIndex]);
        }

        if (pPipelineState->flag == PIPELINE_FLAG::SHADOW)
        {
            uint32_t blockIndex = glGetUniformBlockIndex(m_CurrentShader, "ShadowMapConstants");
            assert(blockIndex != GL_INVALID_INDEX);

            int32_t blockSize;

            glGetActiveUniformBlockiv(m_CurrentShader, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

            assert(blockSize >= sizeof(ShadowMapConstants));

            glUniformBlockBinding(m_CurrentShader, blockIndex, 13);
            glBindBufferBase(GL_UNIFORM_BUFFER, 13, m_uboShadowMatricesConstant[frame.frameIndex]);
        }
    }

    void OpenGLGraphicsManager::BeginFrame(const Frame &frame)
    {
        // Set the color to clear the screen to.
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        // Clear the screen and depth buffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SetPerFrameConstants(frame.frameContext);
        SetLightInfo(frame.lightInfo);
    }

    void OpenGLGraphicsManager::EndFrame(const Frame &frame)
    {
        GraphicsManager::EndFrame(frame);
    }

    void OpenGLGraphicsManager::EndScene(const Scene &scene)
    {
        for (int i = 0; i < m_Frames.size(); i++)
        {
            auto &batchContexts = m_Frames[i].batchContexts;

            for (auto &dbc : batchContexts)
            {
                glDeleteVertexArrays(1, &std::dynamic_pointer_cast<OpenGLDrawBatchContext>(dbc)->vao);
            }

            batchContexts.clear();

            if (m_uboDrawFrameConstant[i])
            {
                glDeleteBuffers(1, &m_uboDrawFrameConstant[i]);
                m_uboDrawFrameConstant[i] = 0;
            }

            if (m_uboDrawBatchConstant[i])
            {
                glDeleteBuffers(1, &m_uboDrawBatchConstant[i]);
                m_uboDrawBatchConstant[i] = 0;
            }

            if (m_uboLightInfo[i])
            {
                glDeleteBuffers(1, &m_uboLightInfo[i]);
                m_uboLightInfo[i] = 0;
            }

            if (m_uboShadowMatricesConstant[i])
            {
                glDeleteBuffers(1, &m_uboShadowMatricesConstant[i]);
                m_uboShadowMatricesConstant[i] = 0;
            }

#if defined(RK_DEBUG)
            if (m_uboDebugConstant[i])
            {
                glDeleteBuffers(1, &m_uboDebugConstant[i]);
                m_uboDebugConstant[i] = 0;
            }
#endif
        }

        if (m_SkyBoxDrawBatchContext.vao)
        {
            glDeleteVertexArrays(1, &m_SkyBoxDrawBatchContext.vao);
            m_SkyBoxDrawBatchContext.vao = 0;
        }

#if defined(RK_DEBUG)
        m_DebugDrawBatchContext.clear();

        for (auto &buf : m_DebugBuffers)
        {
            glDeleteBuffers(1, &buf);
        }
#endif

        for (auto &buf : m_Buffers)
        {
            glDeleteBuffers(1, &buf);
        }

        for (auto &it : m_Textures)
        {
            GLuint texture_id = static_cast<GLuint>(it.second.texture);
            glDeleteTextures(1, &texture_id);
        }

        m_Buffers.clear();
        m_Textures.clear();

        GraphicsManager::EndScene(scene);
    }

    void OpenGLGraphicsManager::SetPerFrameConstants(const DrawFrameContext &context)
    {
        if (!m_uboDrawFrameConstant[m_nFrameIndex])
        {
            glGenBuffers(1, &m_uboDrawFrameConstant[m_nFrameIndex]);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, m_uboDrawFrameConstant[m_nFrameIndex]);

        auto constants = static_cast<PerFrameConstants>(context);

        glBufferData(GL_UNIFORM_BUFFER, kSizePerFrameConstantBuffer, &constants, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void OpenGLGraphicsManager::SetPerBatchConstants(const DrawBatchContext &context)
    {
        if (!m_uboDrawBatchConstant[m_nFrameIndex])
        {
            glGenBuffers(1, &m_uboDrawBatchConstant[m_nFrameIndex]);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, m_uboDrawBatchConstant[m_nFrameIndex]);

        const auto &constant = static_cast<const PerBatchConstants &>(context);

        glBufferData(GL_UNIFORM_BUFFER, kSizePerBatchConstantBuffer, &constant, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void OpenGLGraphicsManager::SetLightInfo(const LightInfo &lightInfo)
    {
        if (!m_uboLightInfo[m_nFrameIndex])
        {
            glGenBuffers(1, &m_uboLightInfo[m_nFrameIndex]);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, m_uboLightInfo[m_nFrameIndex]);

        glBufferData(GL_UNIFORM_BUFFER, kSizeLightInfo, &lightInfo, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

        // Bind the vertex array object to store all the buffers and vertex
        // attributes we create here.
        glBindVertexArray(vao);

        uint32_t buffer_id[2];

        // Generate an ID for the vertex buffer.
        glGenBuffers(2, buffer_id);

        // Bind the vertex buffer and load the vertex (position) data into the
        // vertex buffer.
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);

        // Bind the vertex buffer and load the vertex (uv) data into the vertex
        // buffer.
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);

        glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);

        glDrawArrays(GL_TRIANGLE_STRIP, 0x00, 4);

        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(2, buffer_id);
    }

    texture_id OpenGLGraphicsManager::GenerateCubeShadowMapArray(const uint32_t width, const uint32_t height,
                                                                 const uint32_t count)
    {
        texture_id result;

        uint32_t shadowMap;

        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowMap);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT24, width, height, count * 6);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

        // register the shadow map
        result.texture = static_cast<intptr_t>(shadowMap);
        result.width = width;
        result.height = height;
        result.depth = count;
        result.index = 0;

        return result;
    }

    texture_id OpenGLGraphicsManager::GenerateShadowMapArray(const uint32_t width, const uint32_t height,
                                                             const uint32_t count)
    {
        texture_id result;

        uint32_t shadowMap;

        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMap);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT24, width, height, count);

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        // register the shadow map
        result.texture = static_cast<intptr_t>(shadowMap);
        result.width = width;
        result.height = height;
        result.depth = count;
        result.index = 0;

        return result;
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
        auto event = static_cast<WindowResizeEvent*>(e.get());
        return Resize(event->GetWidth(), event->GetHeight());
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
} // namespace Rocket