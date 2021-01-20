#pragma once
#include "RKConfig.h"

#include <string>
#include <utility>
#include <optional>
#include <functional>

static const std::string ProjectSourceDir = PROJECT_SOURCE_DIR;
static const std::string RenderAPI = RENDER_API;

#define Interface class
#define implements public
#define inheritance public

#define RK_EXPAND_MACRO(x) x
#define RK_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)
#define RK_BIND_EVENT_FN(fn) [](auto &&... args) -> decltype(auto) \
		{ return fn(std::forward<decltype(args)>(args)...); }
#define RK_BIND_EVENT_FN_CLASS(fn) [this](auto &&... args) -> decltype(auto) \
		{ return this->fn(std::forward<decltype(args)>(args)...); }
#define RK_BIND_EVENT_FN_CLASS_PTR(pointer, fn) [pointer](auto &&... args) -> decltype(auto) \
		{ return pointer->fn(std::forward<decltype(args)>(args)...); }

#include "Core/Config.h"
#include "Core/Template.h"
#include "Core/Assert.h"
#include "Core/Log.h"
