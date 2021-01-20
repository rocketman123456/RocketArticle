# Rocket (一) 搭建框架<br>
本节主要介绍建立第一个窗口的部分。在前期的开发中，渲染API将暂定为**OpenGL**，主要原因是其跨平台的能力以及比其他的图形API更加简单。<br>
窗口的创建我选择了[glfw](https://www.glfw.org/)库，其具有跨平台的能力。当然，在需要的时候，我们也可以自己创建窗口，以完成代码移植的操作。<br>
为了能够进行跨平台的包管理，我这里采用了[vcpkg](https://github.com/microsoft/vcpkg)进行cmake包的安装与管理。<br>
## 1. 构建Core Header<br>
首先在头文件中进行必要的宏定义，由CMakeLists.txt中的根据平台以及编译版本自动选择。
```
#pragma once
#include "GECore/Basic.h"

#include "RKConfig.h"
static const std::string ProjectSourceDir = PROJECT_SOURCE_DIR;
static const std::string RenderAPI = RENDER_API;

#ifdef RK_DEBUG
#if defined(PLATFORM_WINDOWS)
#define RK_DEBUGBREAK() __debugbreak()
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_APPLE)
#include <signal.h>
#define RK_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define RK_ENABLE_ASSERTS
#else
#define RK_DEBUGBREAK()
#endif

// These just for glfw
#if defined(RK_OPENGL)
#define GLFW_INCLUDE_NONE
#elif defined(RK_VULKAN)
#define GLFW_INCLUDE_NONE
#elif defined(RK_METAL)
//#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#define RK_EXPAND_MACRO(x) x
#define RK_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)
#define RK_BIND_EVENT_FN(fn) [this](auto &&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define Interface class
#define implements public
#define inheritance public

namespace Rocket
{
	template <typename Base, typename T>
	inline bool InstanceOf (const T *)
	{
		return std::is_base_of<Base, T>::value;
	}

	template <typename T>
	using Scope = std::unique_ptr<T>;
	template <typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args &&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;
	template <typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args &&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using StoreRef = std::weak_ptr<T>;
} // namespace Rocket
```
## 2. 开始Runtime Module
在程序运行时，基本的组件就是不同的Modules，创建一个基类，所有的Module将从这里派生。
`IRuntimeModule.h`
```
#pragma once
#include "Core/Core.h"
#include "Utils/Timestep.h"

namespace Rocket
{
    Interface IRuntimeModule
    {
    public:
        IRuntimeModule(const std::string &name = "IRuntimeModule") : m_Name(name) {}
        virtual ~IRuntimeModule(){};

        virtual int Initialize() = 0;
        virtual void Finalize() = 0;

        virtual void Tick(Timestep ts) = 0;

        // For Debug
        [[nodiscard]] const std::string &GetName() const { return m_Name; }

    private:
        std::string m_Name;
    };
}
```
从这个类继续派生出Application，所有的初始化，终止以及循环的功能，都将在Application类中实现。
`IApplication.h`
```
#pragma once
#include "Interface/IRuntimeModule.h"
#include "Event/Event.h"

namespace Rocket
{
    Interface IApplication : implements IRuntimeModule
    {
    public:
        IApplication(const std::string &name = "IApplication") : IRuntimeModule(name) {}
        virtual ~IApplication() = default;

        virtual void LoadConfig() = 0;

        virtual void PreInitialize() = 0;
        virtual void PostInitialize() = 0;

        virtual void PreInitializeModule() = 0;
        virtual int InitializeModule() = 0;
        virtual void PostInitializeModule() = 0;
        virtual void FinalizeModule() = 0;

        virtual void TickModule() = 0;
        virtual void Tick() = 0;
        virtual void Tick(Timestep ts) override {}
        virtual void OnEvent(Event & event) = 0;
        
        bool IsRunning() { return m_IsRunning; }
        void SetRunningState(bool state) { m_IsRunning = state; }
        void Close() { SetRunningState(false); }
    protected:
        bool m_IsRunning = true;
    };

    IApplication *CreateApplicationInstance();
} // namespace Rocket
```
`Application.h`
```
#pragma once
#include "Interface/IApplication.h"

namespace Rocket
{
    class Application : implements IApplication
    {
    public:
        Application(const std::string &name = "Application") : IApplication(name) {}
        virtual ~Application() = default;

        virtual void PreInitialize() override {}
        virtual int Initialize() override;
        virtual void PostInitialize() override {}
        virtual void Finalize() override;

        virtual void PreInitializeModule() override {}
        virtual int InitializeModule() override;
        virtual void PostInitializeModule() override {}
        virtual void FinalizeModule() override;

        void PushModule(IRuntimeModule * module);

        virtual void TickModule() override;
        virtual void Tick() override;

        static Application &Get() { return *s_Instance; }
    protected:
        bool m_Running = true;
        bool m_Minimized = false;
        bool m_Parallel = true;

        std::chrono::steady_clock m_Clock;
        std::chrono::duration<double> m_Duration;
        std::chrono::time_point<std::chrono::steady_clock> m_CurrentTime;
        std::chrono::time_point<std::chrono::steady_clock> m_LastTime;

        std::vector<IRuntimeModule *> m_Modules;
    private:
        static Application *s_Instance;
    };

    // Implement This Function to Create Different Applications
    Application* CreateApplication();
}
```
`Application.cpp`
```
#include "Module/Application.h"

namespace Rocket
{
    Application* Application::s_Instance = nullptr;

    int Application::InitializeModule()
    {
        RK_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        int ret = 0;
        for (auto &module : m_Modules)
        {
            if ((ret = module->Initialize()) != 0)
            {
                RK_CORE_ERROR("Failed. err = {0}", ret);
                return ret;
            }
        }

        return ret;
    }

    void Application::FinalizeModule()
    {
        for (auto &module : m_Modules)
        {
            module->Finalize();
            delete module;
            module = nullptr;
        }
        m_Modules.clear();
    }

    int Application::Initialize()
    {
        m_CurrentTime = m_Clock.now();
        m_LastTime = m_CurrentTime;

        return 0;
    }

    void Application::Finalize()
    {
    }

    void Application::PushModule(IRuntimeModule *module)
    {
        RK_PROFILE_FUNCTION();
        m_Modules.push_back(module);
    }

    void Application::Tick()
    {
        // Calculate Delta Time
        m_LastTime = m_CurrentTime;
        m_CurrentTime = m_Clock.now();
        m_Duration = m_CurrentTime - m_LastTime;
    }

    void Application::TickModule()
    {
        for (auto &module : m_Modules)
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
