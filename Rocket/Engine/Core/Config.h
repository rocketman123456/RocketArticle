#pragma once

#include <string>
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
    #define GLFW_INCLUDE_NONE
    //#define GLFW_EXPOSE_NATIVE_COCOA
#endif

// EnTT
//#define ENTT_USE_ATOMIC

#if defined(RK_PROFILE)
    #if defined(RK_OPENGL)
        #define RMT_USE_OPENGL 1
    #endif
    #include <Remotery.h>
    #define PROFILE_ENTRY() Remotery* rmt; rmt_CreateGlobalInstance(&rmt);
    #define PROFILE_SET_THREAD(x) rmt_SetCurrentThreadName(#x)
    #define PROFILE_SCOPE_CPU(x, y) rmt_ScopedCPUSample(x, y)
    #define PROFILE_BEGIN_CPU_SAMPLE(x, y) rmt_BeginCPUSample(x, y)
    #define PROFILE_END_CPU_SAMPLE() rmt_EndCPUSample();
    #define PROFILE_EXIT() rmt_DestroyGlobalInstance(rmt)
    #if defined(RK_OPENGL)
        #define PROFILE_BIND_OPENGL() rmt_BindOpenGL()
        #define PROFILE_UNBIND_OPENGL() rmt_UnbindOpenGL()
        #define PROFILE_SCOPE_OPENGL(x) rmt_ScopedOpenGLSample(x)
        #define PROFILE_BEGIN_OPENGL(x) rmt_BeginOpenGLSample(x)
        #define PROFILE_END_OPENGL() rmt_EndOpenGLSample()
    #endif
#else
    #define PROFILE_ENTRY()
    #define PROFILE_SET_THREAD(x)
    #define PROFILE_SCOPE_CPU(x, y)
    #define PROFILE_BEGIN_CPU_SAMPLE(x, y)
    #define PROFILE_END_CPU_SAMPLE()
    #define PROFILE_EXIT()
    #if defined(RK_OPENGL)
        #define PROFILE_BIND_OPENGL()
        #define PROFILE_UNBIND_OPENGL()
        #define PROFILE_SCOPE_OPENGL(x)
        #define PROFILE_BEGIN_OPENGL(x)
        #define PROFILE_END_OPENGL()
    #endif
#endif
