#include "enginepch.h"
#include "Engine/Core/Application.h"

#include "Engine/Core/Layer.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Profiler.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Utils/PlatformUtils.h"
#include "Engine/ImGui/ImGuiLayer.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Reflection/Registry.h"
#include "Engine/ScriptEngine/ScriptServer.h"

namespace Engine
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		ENGINE_PROFILE_FUNCTION();

		ENGINE_CORE_ASSERT(!s_Instance, "Application already exists !");

		s_Instance = this;

		// Set working directory here
		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);
		m_Window = Window::Create(WindowProps(m_Specification.Name));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init(RenderingAPI::Vulkan, m_Window.get());

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		Registry::RegisterEngineClasses();
		ScriptServer::Init();
	}

	Application::~Application()
	{
		ENGINE_PROFILE_FUNCTION();

		//ScriptEngine::Shutdown();
		Renderer::Shutdown();
	}

	void Application::Run()
	{
		ENGINE_PROFILE_FUNCTION();

		while (m_Running)
		{
			ENGINE_PROFILE_SCOPE("RunLoop");

			float time = Time::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			ExecuteMainThreadQueue();

			if (!m_Minimized)
			{
				{
					ENGINE_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}
			}

			{
				ENGINE_PROFILE_SCOPE("LayerStack OnImGuiRender");

				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
		ENGINE_CORE_TRACE("Closing Application...");
	}

	void Application::OnEvent(Event& ev)
	{
		ENGINE_PROFILE_FUNCTION();

		EventDispatcher dispatcher(ev);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResized));

		//ENGINE_CORE_TRACE("{}", ev.ToString());

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(ev);
			if (ev.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		m_MainThreadQueue.emplace_back(function);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResized(WindowResizeEvent& ev)
	{
		ENGINE_PROFILE_FUNCTION();

		if (ev.GetWidth() == 0 || ev.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;

		Renderer::OnWindowResize(ev.GetWidth(), ev.GetHeight());

		return false;
	}

	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		for (auto& func : m_MainThreadQueue)
			func();

		m_MainThreadQueue.clear();
	}

}