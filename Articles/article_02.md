# Rocket 引擎系列（二）<br>
这一章主要介绍我们的框架设计，当前设计的引擎框架还是单线程版本，暂时未引入整体的多线程架构。本章的主要参考是[Game Engine Architecture](https://www.gameenginebook.com/)和[高品质游戏开发](https://www.zhihu.com/column/c_119702958)。<br>
# 二、项目框架<br>
本引擎主体分为两个部分，用户定义的应用层和引擎自身的不同模块组件，比如窗口管理，进程管理等。<br>
对于整个程序来说，主体循环主要是以下函数：
```
int main(int argc, char **argv)
{
    auto app = Rocket::CreateApplication();

    app->PreInitializeModule();
    if(app->InitializeModule() != 0)
        return 1;
    app->PostInitializeModule();

    app->PreInitialize();
    if(app->Initialize() != 0)
        return 1;
    app->PostInitialize();

    while(app->GetIsRunning())
    {
        app->Tick();
        app->TickModule();
    }

    app->Finalize();
    app->FinalizeModule();
	delete app;
    return 0;
}
```
其中，`PreInitializeModule` `InitializeModule` `PostInitializeModule` `TickModule` `FinalizeModule` 是模块对应的部分，`PreInitialize` `Initialize` `PostInitialize` `Tick` `Finalize` 对应的是用户定义的应用层的部分。<br>
每一个模块显式初始化与终止，具有明确的顺序。在这样的条件下，可以尽量避免由于依赖关系引起的初始化失败的问题。模块属于引擎的底层，由用户定义编写的部分为应用层，这样分层的应用结构可以快速添加新的应用。<br>
在这之中，只有`Pre`，`Post`，`Finalize`可以被继承修改，主体循环被确定在`Application`类里面，不应该由引擎外部进行修改。这些由用户定义的函数可以自由添加需要的模块，以及对应的应用层。
```
virtual void PreInitializeModule();
virtual void PostInitializeModule();

virtual void PreInitialize();
virtual void PostInitialize();

virtual void Finalize();
virtual void FinalizeModule();
```
对于一切需要在引擎主循环中运行的模块，都设置为`IRuntimeModule`<br>
```
#pragma once
#include "GECore/Core.h"
#include "GEUtils/Timestep.h"

namespace Rocket {
    Interface IRuntimeModule{
    public:
        IRuntimeModule(const std::string& name = "IRuntimeModule") : m_Name(name) {}
        virtual ~IRuntimeModule() {};

        virtual int Initialize() = 0;
        virtual void Finalize() = 0;

        virtual void Tick(Timestep ts) = 0;

        // For debug
        const std::string& GetName() const { return m_Name; }
    private:
        std::string m_Name;
    };
}
```
针对`Application`专门设计类`IApplication`
```
#pragma once
#include "GEInterface/IRuntimeModule.h"

namespace Rocket {
    Interface IApplication : implements IRuntimeModule
    {
    public:
        virtual void PreInitializeModule() = 0;
        virtual int InitializeModule() = 0;
        virtual void PostInitializeModule() = 0;
        virtual void FinalizeModule() = 0;

        virtual void PreInitialize() = 0;
        virtual int Initialize() = 0;
        virtual void PostInitialize() = 0;
        virtual void Finalize() = 0;

        // One cycle of the main loop
        virtual void Tick() = 0;

        virtual void OnEvent(Event& event) = 0;

        virtual bool IsQuit() = 0;
    };

    IApplication* CreateApplicationInstance();
}
```
这就具备了搭建框架的初始条件，下一章将会开始搭建第一个窗口，开始进行渲染。整个项目位于[github](https://github.com/rocketman123456/RocketArticle)。