# Rocket (二) 创建窗口<br>
在上一节中，我们建立了Application类，本节就开始创建第一个窗口。
## 1. 窗口类<br>
为了封装window的操作，创建了window基类<br>
`Window.h`
```
#pragma once
#include "Core/Core.h"
#include "Event/Event.h"

namespace Rocket
{
    struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string &title = "Rocket Engine",
					uint32_t width = 1600,
					uint32_t height = 900)
			: Title(title), Width(width), Height(height) {}
	};

    // Interface representing a desktop system based Window
	Interface Window
	{
	public:
		using EventCallbackFn = std::function<void(Event &)>;
		virtual ~Window() = default;

        virtual void Initialize() = 0;
        virtual void Finalize() = 0;

		virtual void PollEvent() = 0;
		virtual void Update() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn &callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Ref<Window> Create(const WindowProps &props = WindowProps());
    protected:
        struct WindowData
        {
            std::string Title;
            uint32_t Width = 0, Height = 0;
            float xScale = 1.0f, yScale = 1.0f;
            bool VSync = true;
            EventCallbackFn EventCallback;
        };
	};
}
```
其中，事件处理将放到后续章节介绍。<br>
`Window.cpp`
```
#include "Common/Window.h"
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_APPLE) || defined(PLATFORM_LINUX)
#include "Common/WindowImplement.h"
#endif

namespace Rocket
{
	Ref<Window> Window::Create(const WindowProps &props)
	{
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_APPLE) || defined(PLATFORM_LINUX)
		return CreateRef<WindowImplement>(props);
#else
		RK_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

} // namespace Rocket
```
由于本篇采用的是glfw创建窗口，所以通过Window基类来派生WindowImplement实现glfw创建窗口<br>
`WindowImplement.h`
```
#pragma once
#include "Common/Window.h"

#include <GLFW/glfw3.h>

namespace Rocket
{
    class WindowImplement : implements Window
	{
	public:
		WindowImplement(const WindowProps &props) : m_Props(props) {}
		virtual ~WindowImplement() = default;

        virtual void Initialize() final;
		virtual void Finalize() final;

		virtual void PollEvent() final;
		virtual void Update() final;

		uint32_t GetWidth() const final { return m_Data.Width; }
		uint32_t GetHeight() const final { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn &callback) final { m_Data.EventCallback = callback; }
		bool IsVSync() const final { return m_Data.VSync; }
		void SetVSync(bool enabled) final;

		virtual void* GetNativeWindow() const final { return m_Window; }
    private:
		virtual void SetCallback();

	private:
		WindowData m_Data;
        WindowProps m_Props;
		GLFWwindow* m_Window;
	};
}
```
`WindowImplement.cpp`
```
#include "Common/WindowImplement.h"
#include "Event/ApplicationEvent.h"

#include <glad/glad.h>

namespace Rocket
{
    static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char *description)
	{
		RK_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	void WindowImplement::Initialize()
	{
		m_Data.Title = m_Props.Title;
		m_Data.Width = m_Props.Width;
		m_Data.Height = m_Props.Height;

		RK_CORE_INFO("Creating window {0} ({1}, {2})", m_Props.Title, m_Props.Width, m_Props.Height);

		if (s_GLFWWindowCount == 0)
		{
			int success = glfwInit();
			RK_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
#if defined(RK_OPENGL)
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(PLATFORM_APPLE)
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#if defined(RK_DEBUG)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
#elif defined(RK_VULKAN) || defined(RK_METAL)
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
			glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);
			glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

			m_Window = glfwCreateWindow((int)m_Props.Width, (int)m_Props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		SetCallback();
	}

	void WindowImplement::SetCallback()
	{
		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height) {
			RK_CORE_TRACE("glfwSetWindowSizeCallback");
		});

		glfwSetWindowContentScaleCallback(m_Window, [](GLFWwindow *window, float xscale, float yscale) {
			RK_CORE_TRACE("glfwSetWindowContentScaleCallback");
		});

		glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow *window) {
			RK_CORE_TRACE("glfwSetWindowRefreshCallback");
		});

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow *window, int width, int height) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;
			WindowResizeEvent event(width, height, 1.0f, 1.0f);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

			switch (action)
			{
                case GLFW_PRESS:
                {
                    //KeyPressedEvent event(key, 0);
                    //data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    //KeyReleasedEvent event(key);
                    //data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    //KeyPressedEvent event(key, 1);
                    //data.EventCallback(event);
                    break;
                }
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow *window, uint32_t keycode) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			//KeyTypedEvent event(keycode);
			//data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

			switch (action)
			{
                case GLFW_PRESS:
                {
                    //MouseButtonPressedEvent event(button);
                    //data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    //MouseButtonReleasedEvent event(button);
                    //data.EventCallback(event);
                    break;
                }
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xOffset, double yOffset) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			//MouseScrolledEvent event((float)xOffset, (float)yOffset);
			//data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xPos, double yPos) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			//MouseMovedEvent event((float)xPos, (float)yPos);
			//data.EventCallback(event);
		});
	}

	void WindowImplement::Finalize()
	{
		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void WindowImplement::PollEvent()
	{
		glfwPollEvents();
	}

	void WindowImplement::Update()
	{
#if defined(RK_OPENGL)
        glfwSwapBuffers(m_Window);
#endif
	}

	void WindowImplement::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}
}
```
通过WindowManager进行窗口管理<br>
`WindowManager.h`
```
#pragma once
#include "Interface/IRuntimeModule.h"
#include "Common/Window.h"

namespace Rocket
{
    class WindowManager : implements IRuntimeModule
    {
    public:
        WindowManager() : IRuntimeModule("WindowManager") {}
        virtual ~WindowManager() = default;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual void Tick(Timestep ts) final;

        const Ref<Window> GetWindow() { return m_Window; }
    private:
        Ref<Window> m_Window;
    };
}
```
`WindowManager.cpp`
```
#include "Module/WindowManager.h"

namespace Rocket
{
    int WindowManager::Initialize()
    {
        WindowProps prop;
        m_Window = Window::Create(prop);
        m_Window->Initialize();
        return 0;
    }

    void WindowManager::Finalize()
    {
        m_Window->Finalize();
    }

    void WindowManager::Tick(Timestep ts)
    {
        m_Window->Update();
    }
}
```
最后创建SimpleApp来创造第一个窗口，当然此时由于未创建渲染API的部分，在运行时会报错，下一节将解决这个问题。
`SimpleApp.h`
```
#pragma once
#include "Module/Application.h"

namespace Rocket
{
    class SimpleApp : implements Application
    {
    public:
        SimpleApp() : Application("SimpleApp") {}

        virtual void PreInitializeModule() override;
        virtual void PostInitializeModule() override;

        virtual void PreInitialize() override;
        virtual void PostInitialize() override;
    };
}
```
`SimpleApp.cpp`
```
#include "SimpleApp.h"
#include "Module/WindowManager.h"

namespace Rocket
{
    Application* CreateApplication()
    {
        return new SimpleApp();
    }

    void SimpleApp::PreInitializeModule()
    {
        PushModule(new WindowManager());
    }

    void SimpleApp::PostInitializeModule()
    {
    }

    void SimpleApp::PreInitialize()
    {
    }

    void SimpleApp::PostInitialize()
    {
    }
}
```