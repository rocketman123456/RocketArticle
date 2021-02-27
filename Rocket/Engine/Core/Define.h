#pragma once

#define Interface class
#define implements public
#define inheritance public

#define RK_EXPAND_MACRO(x) x
#define RK_STRINGIFY_MACRO(x) #x

#define RK_BIT(x) (1 << x)

#ifndef RK_ALIGN
#define RK_ALIGN(x, a) (((x) + ((a)-1)) & ~((a)-1))
#endif

#include <cstdint>
using four_char_enum = int32_t;
#define ENUM(e) enum class e : four_char_enum
