#pragma once

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