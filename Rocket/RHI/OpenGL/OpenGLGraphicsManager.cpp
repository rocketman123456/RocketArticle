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
        m_WindowHandle = static_cast<GLFWwindow*>(g_WindowManager->GetWindow()->GetNativeWindow());

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

        glfwSetFramebufferSizeCallback(m_WindowHandle, [](GLFWwindow *window, int width, int height) {
            // make sure the viewport matches the new window dimensions; note that width and 
            // height will be significantly larger than specified on retina displays.
            glViewport(0, 0, width, height);
		});

        return 0;
    }

    void OpenGLGraphicsManager::Finalize()
    {
    }

    void OpenGLGraphicsManager::Tick(Timestep ts)
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(m_WindowHandle);
    }
}