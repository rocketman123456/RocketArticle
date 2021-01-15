#include "Module/EventManager.h"
#include "Module/WindowManager.h"

#include <GLFW/glfw3.h>

namespace Rocket
{
    EventManager* GetEventManager()
    {
        return new EventManager();
    }

    int EventManager::Initialize()
    {
        m_WindowHandle = static_cast<GLFWwindow*>(g_WindowManager->GetNativeWindow());

        glfwMakeContextCurrent(m_WindowHandle);
        glfwSetWindowUserPointer(m_WindowHandle, &m_Data);

        SetupCallback();

        this->SetEventCallback(RK_BIND_EVENT_FN(EventManager::OnEvent));

        return 0;
    }

    void EventManager::SetupCallback()
    {
        // Set GLFW callbacks
		glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow *window, int width, int height) {
			RK_CORE_TRACE("glfwSetWindowSizeCallback");
		});

		glfwSetWindowContentScaleCallback(m_WindowHandle, [](GLFWwindow *window, float xscale, float yscale) {
			RK_CORE_TRACE("glfwSetWindowContentScaleCallback");
		});

		glfwSetWindowRefreshCallback(m_WindowHandle, [](GLFWwindow *window) {
			RK_CORE_TRACE("glfwSetWindowRefreshCallback");
		});

		glfwSetFramebufferSizeCallback(m_WindowHandle, [](GLFWwindow *window, int width, int height) {
            RK_CORE_TRACE("glfwSetFramebufferSizeCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			WindowResizeEvent event(width, height, 1.0f, 1.0f);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow *window) {
            RK_CORE_TRACE("glfwSetWindowCloseCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            RK_CORE_TRACE("glfwSetKeyCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			switch (action)
			{
                case GLFW_PRESS:
                {
                    //KeyPressedEvent event(key, 0);
                    //data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    //KeyReleasedEvent event(key);
                    //data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    //KeyPressedEvent event(key, 1);
                    //data.EventCallback(event);
                    break;
                }
			}
		});

		glfwSetCharCallback(m_WindowHandle, [](GLFWwindow *window, uint32_t keycode) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			//KeyTypedEvent event(keycode);
			//data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow *window, int button, int action, int mods) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

			switch (action)
			{
                case GLFW_PRESS:
                {
                    //MouseButtonPressedEvent event(button);
                    //data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    //MouseButtonReleasedEvent event(button);
                    //data.EventCallback(event);
                    break;
                }
			}
		});

		glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow *window, double xOffset, double yOffset) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			//MouseScrolledEvent event((float)xOffset, (float)yOffset);
			//data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow *window, double xPos, double yPos) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			//MouseMovedEvent event((float)xPos, (float)yPos);
			//data.EventCallback(event);
		});
    }

    void EventManager::Finalize()
    {
    }

    void OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
    }

    void EventManager::Tick(Timestep ts)
    {
        glfwPollEvents();
    }
}