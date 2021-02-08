#include "GLFWWindow/WindowImplement.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace Rocket;

static uint8_t s_GLFWWindowCount = 0;

static void GLFWErrorCallback(int error, const char *description)
{
	RK_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

void WindowImplement::Initialize()
{
	RK_CORE_INFO("Creating Window {0} ({1}, {2})", m_Props.Title, m_Props.Width, m_Props.Height);

	if (s_GLFWWindowCount == 0)
	{
		int success = glfwInit();
		RK_CORE_ASSERT(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
	}

#if defined(RK_OPENGL)
#if defined(PLATFORM_APPLE)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#elif defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(PLATFORM_APPLE)
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#elif defined(RK_VULKAN) || defined(RK_METAL)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
#if defined(RK_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	m_Window = glfwCreateWindow((int)m_Props.Width, (int)m_Props.Height, m_Props.Title.c_str(), nullptr, nullptr);
	++s_GLFWWindowCount;

	// Get Frame Buffer Size
	glfwGetFramebufferSize(m_Window, &m_Props.Width, &m_Props.Height);
}

void WindowImplement::Finalize()
{
	glfwDestroyWindow(m_Window);
	--s_GLFWWindowCount;

	if (s_GLFWWindowCount == 0)
	{
		glfwTerminate();
	}
}
