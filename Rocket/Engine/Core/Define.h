#pragma once

#define Interface class
#define implements public
#define inheritance public

#define RK_EXPAND_MACRO(x) x
#define RK_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#ifdef ALIGN
#undef ALIGN
#endif
#define ALIGN(x, a) (((x) + ((a)-1)) & ~((a)-1))

#include <cstdint>
using four_char_enum = int32_t;
#define ENUM(e) enum class e : four_char_enum
