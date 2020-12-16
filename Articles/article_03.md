# Rocket 引擎系列（三）<br>
本项目面对的是多平台，具体是指Mac，Windows和Linux，为了能够使代码顺利在不同平台下编译，我们需要对CMake进行配置，使其能够检测不同平台，并对其进行处理。
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
message(STATUS "Build System Set to ${CMAKE_BUILD_TYPE}")

message(STATUS "operation system is ${CMAKE_SYSTEM}")
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "current platform: Linux ")
    set(Linux 1)
    add_definitions(-DPLATFORM_LINUX)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "current platform: Windows")
    set(Windows 1)
    add_definitions(-DPLATFORM_WINDOWS)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(STATUS "current platform: Apple Darwin")
    set(Apple 1)
    add_definitions(-DPLATFORM_APPLE)
else ()
    message(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
endif ()

message(STATUS "###################################")

message(STATUS "Render API Set to ${RENDER_API}")
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

message(STATUS "###################################")

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