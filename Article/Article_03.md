# Rocket (三) 图形管理<br>
本节将会完成建立第一个窗口的功能，能够成功打开窗口，但是缺乏对应的事件处理，所以无法关闭。这些问题将会后续一一解决。<br>
## 1. Graphics Manager<br>
当前，Graphics Manager的功能主要是OpenGL的加载功能，并且在屏幕中渲染指定的颜色。渲染器中垂直同步等选项现在是硬编码的数值，后续将会替换成从配置文件中加载<br>
`GraphicsManager.h`
```
#pragma once
#include "Interface/IRuntimeModule.h"

namespace Rocket
{
    class GraphicsManager : inheritance IRuntimeModule
    {
    public:
        GraphicsManager(const std::string &name = "IRuntimeModule");
        virtual ~GraphicsManager() = default;
    };

    GraphicsManager* GetGraphicsManager();
    extern GraphicsManager* g_GraphicsManager;
}
```
`GraphicsManager.cpp`
```
#include "Module/GraphicsManager.h"

namespace Rocket
{
    GraphicsManager::GraphicsManager(const std::string &name) 
        : IRuntimeModule(name) 
    {
    }
}
```
`OpenGLGraphicsManager.h`
```
#pragma once
#include "Module/GraphicsManager.h"

struct GLFWwindow;

namespace Rocket
{
    class OpenGLGraphicsManager : implements GraphicsManager
    {
    public:
        OpenGLGraphicsManager() : GraphicsManager("OpenGLGraphicsManager") {}
        ~OpenGLGraphicsManager() = default;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual void Tick(Timestep ts) final;
    private:
        GLFWwindow* m_WindowHandle = nullptr;
        bool m_VSync = true;
    };
}
```
`OpenGLGraphicsManager.cpp`
```
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
```