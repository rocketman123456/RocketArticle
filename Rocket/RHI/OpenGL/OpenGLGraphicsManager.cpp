#include "OpenGL/OpenGLGraphicsManager.h"
#include "Module/WindowManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

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
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(m_WindowHandle);

        PROFILE_END_CPU_SAMPLE();
        PROFILE_END_OPENGL();
    }
}