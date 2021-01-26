#include "OpenGL/OpenGLGraphicsManager.h"
#include "Module/WindowManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static void OpenGLMessageCallback(
    unsigned source, unsigned type, unsigned id, unsigned severity,
    int length, const char *message, const void *userParam)
{
    RK_GRAPHICS_INFO("---------------");
    RK_GRAPHICS_INFO("Debug message ({0})", id);

    switch (source) {
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
    
    switch (type) {
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
    GraphicsManager* GetGraphicsManager()
    {
        return new OpenGLGraphicsManager();
    }

    int OpenGLGraphicsManager::Initialize()
    {
        PROFILE_BIND_OPENGL();

        m_WindowHandle = static_cast<GLFWwindow*>(g_WindowManager->GetNativeWindow());

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
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		}

        glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);

        return 0;
    }

    void OpenGLGraphicsManager::Finalize()
    {
        PROFILE_UNBIND_OPENGL();
    }

    void OpenGLGraphicsManager::Tick(Timestep ts)
    {
        PROFILE_SCOPE_OPENGL(OpenGLTick);
        PROFILE_BEGIN_OPENGL(OpenGLRender);
        PROFILE_BEGIN_CPU_SAMPLE(OpenGLGraphicsManagerUpdate, 0);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SwapBuffers();

        PROFILE_END_CPU_SAMPLE();
        PROFILE_END_OPENGL();
    }

    void OpenGLGraphicsManager::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }

    void OpenGLGraphicsManager::DrawPoint(const Point3D& point, const Vector3f& color)
    {
    }

    void OpenGLGraphicsManager::DrawPointSet(const Point3DSet& point_set, const Vector3f& color)
    {
    }

    void OpenGLGraphicsManager::DrawPointSet(const Point3DSet& point_set, const Matrix4f& trans, const Vector3f& color)
    {
    }

    void OpenGLGraphicsManager::DrawLine(const Point3D& from, const Point3D& to, const Vector3f &color)
    {
    }

    void OpenGLGraphicsManager::DrawLine(const Point3DList& vertices, const Vector3f &color)
    {
    }

    void OpenGLGraphicsManager::DrawLine(const Point3DList& vertices, const Matrix4f& trans, const Vector3f &color)
    {
    }

    void OpenGLGraphicsManager::DrawTriangle(const Point3DList& vertices, const Vector3f &color)
    {
    }

    void OpenGLGraphicsManager::DrawTriangle(const Point3DList& vertices, const Matrix4f& trans, const Vector3f &color)
    {
    }

    void OpenGLGraphicsManager::DrawTriangleStrip(const Point3DList& vertices, const Vector3f &color)
    {
    }
}