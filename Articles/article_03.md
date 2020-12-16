# Rocket 引擎系列（三）<br>
本项目面对的是多平台，具体是指Mac，Windows和Linux，为了能够使代码顺利在不同平台下编译，我们需要对CMake进行配置，使其能够检测不同平台，并对其进行处理。[github](https://github.com/rocketman123456/RocketArticle)
# 1.CMake 配置
- `CMakeLists.txt`
```
cmake_minimum_required (VERSION 2.8.12)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(VCPKG_ROOT "/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "")
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(VCPKG_ROOT "D:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "")
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(VCPKG_ROOT "/Users/developer/Program/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "")
endif ()
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT})

#### Main Project Config ############################
set(RENDER_API "OpenGL")
set(CMAKE_BUILD_TYPE "Debug")
set(PROFILE 0)
#### Main Project Config ############################

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

message(STATUS "###################################")

project( RocketGE )
message(STATUS "CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Project Name ${PROJECT_NAME}")

configure_file (
    "${PROJECT_SOURCE_DIR}/RKConfig.h.in"
    "${PROJECT_SOURCE_DIR}/Rocket/GEEngine/RKConfig.h"
)

message(STATUS "###################################")

if(PROFILE)
    add_definitions(-DRK_PROFILE)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DRK_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRK_RELEASE)
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(Linux 1)
    add_definitions(-DPLATFORM_LINUX)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(Windows 1)
    add_definitions(-DPLATFORM_WINDOWS)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(Apple 1)
    add_definitions(-DPLATFORM_APPLE)
endif ()

if (RENDER_API MATCHES "OpenGL")
    set(OpenGL 1)
    add_definitions(-DRK_OPENGL)
    if(Windows)
        add_definitions(-DHIGH_OPENGL_VERSION)
    elseif(Linux)
        add_definitions(-DHIGH_OPENGL_VERSION)
    endif()
elseif (RENDER_API MATCHES "Vulkan")
    set(Vulkan 1)
    add_definitions(-DRK_VULKAN)
elseif (RENDER_API MATCHES "Metal")
    set(Metal 1)
    add_definitions(-DRK_METAL)
endif()

add_subdirectory( Test )
```
其中主要设定了各种defination，用于在代码中识别不同的编译对象，包括编译平台，渲染API选择等。其中，下面的代码用于生成对应的配置文件：
```
configure_file (
    "${PROJECT_SOURCE_DIR}/RKConfig.h.in"
    "${PROJECT_SOURCE_DIR}/Rocket/GEEngine/RKConfig.h"
)
```
- `RKConfig.h.in`
```
#define PROJECT_SOURCE_DIR "@PROJECT_SOURCE_DIR@"
#define RENDER_API "@RENDER_API@"
#define BUILD_TYPE "@CMAKE_BUILD_TYPE@"
```
在本项目中，渲染暂时只拥有OpenGL一个版本，未来首先会集成Vulkan，并将整个项目进行并行化处理。<br>
# 2.添加第一个Application<br>
- `Application.h`
```
#pragma once

#include "GECore/Core.h"
#include "GEInterface/IApplication.h"
#include "GEWindow/Window.h"
#include "GEEvent/Event.h"

namespace Rocket {
    Interface Application : implements IApplication
    {
    public:
        Application(const std::string& name = "Application") 
            : IApplication(name) { s_Instance = this; }
        virtual ~Application() = default;

        virtual void PreInitialize() override {}
        virtual int Initialize() override;
        virtual void PostInitialize() override {}
        virtual void Finalize() override;

        virtual void PreInitializeModule() override {}
        virtual int InitializeModule() override;
        virtual void PostInitializeModule() override {}
        virtual void FinalizeModule() override;

        virtual void OnEvent(Event& e) override;
        void Close();

        void PushModule(IRuntimeModule* module);

        virtual void TickModule() override;
        virtual void Tick() override;

        inline Window& GetWindow() { return *m_Window; }
        inline bool GetIsRunning() override { return m_Running; }
        static Application& Get() { return *s_Instance; }
    private:
        bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
    private:
        Ref<Window> m_Window;

        std::vector<IRuntimeModule*> m_Modules;

        bool m_Running = true;
        bool m_Minimized = false;
        bool m_Parallel = true;

        std::chrono::steady_clock m_Clock;
        std::chrono::duration<double> m_Duration;
        std::chrono::time_point<std::chrono::steady_clock> m_CurrentTime;
        std::chrono::time_point<std::chrono::steady_clock> m_LastTime;
    private:
        static Application* s_Instance;
    };

    Application* CreateApplication();
}
```
- `Application.cpp`
```
#include "GEModule/Application.h"
#include "GEModule/WindowManager.h"

namespace Rocket
{
    Application *Application::s_Instance = nullptr;

    int Application::Initialize()
    {
        // Initialize Layers
        m_CurrentTime = m_Clock.now();
        m_LastTime = m_CurrentTime;

        return 0;
    }

    void Application::Finalize()
    {
        // Finalize Layers
    }

    int Application::InitializeModule()
    {
        // Initialize Modules
        int ret = 0;
        for (auto& module : m_Modules)
        {
            if ((ret = module->Initialize()) != 0) {
                RK_CORE_ERROR("Failed. err = {0}", ret);
                return ret;
            }
        }

        m_Window = g_WindowManager->GetWindow();
        Renderer::Init();
        return ret;
    }

    void Application::FinalizeModule()
    {
        // Finalize Modules
        for (auto& module : m_Modules)
        {
            module->Finalize();
            delete module;
        }

        Renderer::Shutdown();
    }

    void Application::PushModule(IRuntimeModule* module)
    {
        m_Modules.push_back(module);
    }

    void Application::OnEvent(Event &e)
    {
        // Send Events
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(RK_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(RK_BIND_EVENT_FN(Application::OnWindowResize));
    }

    void Application::Tick()
    {
        // Calculate Delta Time
        m_LastTime = m_CurrentTime;
        m_CurrentTime = m_Clock.now();
        m_Duration = m_CurrentTime - m_LastTime;
        // Poll Event Must Call in Main Thread
        m_Window->PollEvent();
    }

    void Application::TickModule()
    {
        for (auto& module : m_Modules)
        {
            module->Tick(Timestep(m_Duration.count()));
        }
    }

    void Application::Close()
    {
        m_Running = false;
    }
}

```
# 3.添加第一个窗口<br>
- `Window.h`
```
#pragma once

#include "GECore/Core.h"

namespace Rocket {
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Rocket Engine",
			        uint32_t width = 1600,
			        uint32_t height = 900)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	Interface Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void PollEvent() = 0;
		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Ref<Window> Create(const WindowProps& props = WindowProps());
	};

}
```
- `Window.cpp`
```
#include "GEWindow/Window.h"

#if defined(PLATFORM_WINDOWS)
#include "GEWindow/WindowWindows.h"
#elif defined(PLATFORM_APPLE)
#include "GEWindow/WindowApple.h"
#elif defined(PLATFORM_LINUX)
#include "GEWindow/WindowLinux.h"
#endif

namespace Rocket
{
	Ref<Window> Window::Create(const WindowProps& props)
	{
	#if defined(PLATFORM_WINDOWS)
		return CreateRef<WindowWindows>(props);
	#elif defined(PLATFORM_APPLE)
		return CreateRef<WindowApple>(props);
    #elif defined(PLATFORM_LINUX)
        return CreateRef<WindowLinux>(props);
    #else
        RK_CORE_ASSERT(false, "Unknown platform!");
        return nullptr;
    #endif
	}

}
```
- `WindowApple.h`
```
#pragma once

#include "GECore/Core.h"
#include "GEWindow/Window.h"
#include "GEEvent/Event.h"
#include "GERender/GraphicsContext.h"

struct GLFWwindow;

namespace Rocket
{
	class WindowApple : implements Window
	{
	public:
		WindowApple(const WindowProps &props);
		virtual ~WindowApple();

		virtual void PollEvent() override;
		virtual void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn &callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void *GetNativeWindow() const override { return m_Window; }

	private:
		virtual void Init(const WindowProps &props);
		virtual void Shutdown();

	private:
		struct WindowData
		{
			std::string Title;
			uint32_t Width, Height;
			float xScale = 1.0f, yScale = 1.0f;
			bool VSync;
			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		GLFWwindow *m_Window;
		Scope<GraphicsContext> m_Context;
	};

}
```
- `WindowApple.cpp`
```
#include "GEWindow/WindowApple.h"
#include "GEEvent/KeyEvent.h"
#include "GEEvent/MouseEvent.h"
#include "GEEvent/ApplicationEvent.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#ifdef RK_OPENGL
#include "GERender/OpenGLContext.h"
#endif
#ifdef RK_VULKAN
#include "GERender/VulkanContext.h"
#endif
#ifdef RK_METAL
#include "GERender/MetalContext.h"
#endif

namespace Rocket
{
	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char *description)
	{
		//RK_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WindowApple::WindowApple(const WindowProps &props)
	{
		Init(props);
	}

	WindowApple::~WindowApple()
	{
		Shutdown();
	}

	void WindowApple::Init(const WindowProps &props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		RK_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

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
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#elif defined(RK_VULKAN)
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#elif defined(RK_METAL)
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
			glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);
#if defined(RK_DEBUG)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
			//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}

		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height) {
			//RK_CORE_TRACE("glfwSetWindowSizeCallback");
		});

		glfwSetWindowContentScaleCallback(m_Window, [](GLFWwindow* window, float xscale, float yscale){
			RK_CORE_TRACE("glfwSetWindowContentScaleCallback");
		});

		glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow* window){
			RK_CORE_TRACE("glfwSetWindowRefreshCallback");
			//glfwSwapBuffers(window);
		});

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height){
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
			case GLFW_PRESS: {
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE: {
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT: {
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow *window, unsigned int keycode) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS: {
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xOffset, double yOffset) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xPos, double yPos) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WindowApple::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void WindowApple::PollEvent()
	{
		glfwPollEvents();
	}

	void WindowApple::OnUpdate()
	{
		m_Context->SwapBuffers();
	}

	void WindowApple::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowApple::IsVSync() const
	{
		return m_Data.VSync;
	}
}
```