#include "enginepch.h"
#include "Platform/Windows/WindowsWindow.h"

#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Profiler.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

#include <bgfx/bgfx.h>

//#include "Platform/OpenGL/OpenGLContext.h"
//#include "Platform/bgfx/bgfxContext.h"

namespace Engine
{

	static uint8_t s_GLFWWindowCount;

	static void GLFWErrorCallback(int error, const char* description)
	{
		ENGINE_CORE_ERROR("GLFW Error ({}) : {}", error, description);
	}

	Scope<Window> Window::Create(const WindowProps& props /* = WindowProps() */) { return CreateScope<WindowsWindow>(props); }

	WindowsWindow::WindowsWindow(const WindowProps& props) { ENGINE_PROFILE_FUNCTION(); Init(props); }

	WindowsWindow::~WindowsWindow() { ENGINE_PROFILE_FUNCTION(); Shutdown(); }

	void WindowsWindow::Init(const WindowProps& props)
	{
		ENGINE_PROFILE_FUNCTION();
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;


		ENGINE_CORE_INFO("Creating window {} {}x{}", props.Title, props.Width, props.Height);


		if (s_GLFWWindowCount == 0)
		{
			ENGINE_PROFILE_SCOPE("glfwInit");

			int success = glfwInit();
			ENGINE_CORE_ASSERT(success, "Couldn't initialize GLFW!");

			glfwSetErrorCallback(GLFWErrorCallback);
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		{
			ENGINE_PROFILE_SCOPE("glfwCreateWindow");

			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}

		//m_Context = new bgfxContext(m_Window);
		//m_Context->Init();
		// ^

		glfwSetWindowUserPointer(m_Window, &m_Data);

		// Set GLFW Callbacks

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				data.Width = width;
				data.Height = height;

				WindowResizeEvent ev(width, height);
				data.EventCallback(ev);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent ev;
				data.EventCallback(ev);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int modes)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent ev(key);
					data.EventCallback(ev);
					break;
				}
				case  GLFW_RELEASE:
				{
					KeyReleasedEvent ev(key);
					data.EventCallback(ev);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent ev(key, true);
					data.EventCallback(ev);
					break;
				}
				default:
					break;
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypedEvent ev(keycode);
				data.EventCallback(ev);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int modes)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent ev(button);
					data.EventCallback(ev);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent ev(button);
					data.EventCallback(ev);
					break;
				}
				default:
					break;
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent ev((float)xOffset, (float)yOffset);
				data.EventCallback(ev);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent ev((float)xPos, (float)yPos);
				data.EventCallback(ev);
			});

		glfwSetDropCallback(m_Window, [](GLFWwindow* window, int pathCount, const char* paths[])
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				std::vector<std::filesystem::path> filepaths(pathCount);
				for (int i = 0; i < pathCount; i++)
					filepaths[i] = paths[i];
				WindowDropEvent event(std::move(filepaths));
				data.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		ENGINE_PROFILE_FUNCTION();
		//m_Context->Shutdown();
		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
			glfwTerminate();
	}

	void WindowsWindow::OnUpdate()
	{
		ENGINE_PROFILE_FUNCTION();
		glfwPollEvents();
		//m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		ENGINE_PROFILE_FUNCTION();

		m_Data.VSync = enabled;

		int flags = BGFX_RESET_HIDPI;
		if (enabled)
			flags |= BGFX_RESET_VSYNC;

		bgfx::reset(GetWidth(), GetHeight(), flags);
	}

	bool WindowsWindow::IsVSync() const { return m_Data.VSync; }
}