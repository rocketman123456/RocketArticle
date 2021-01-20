#include "Common/Window.h"
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_APPLE) || defined(PLATFORM_LINUX)
#include "GLFWWindow/WindowImplement.h"
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