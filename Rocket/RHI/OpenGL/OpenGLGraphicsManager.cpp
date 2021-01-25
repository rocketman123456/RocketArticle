#include "OpenGL/OpenGLGraphicsManager.h"
#include "Module/WindowManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void OpenGLMessageCallback(
    unsigned source, unsigned type,
    unsigned id, unsigned severity,
    int length, const char *message, const void *userParam)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        RK_CORE_CRITICAL(message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        RK_CORE_ERROR(message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        RK_CORE_WARN(message);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        RK_CORE_TRACE(message);
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

        RK_CORE_INFO("OpenGL Info:");
        RK_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
        RK_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
        RK_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

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