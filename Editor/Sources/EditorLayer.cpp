#include "EditorLayer.h"
#include "Engine/Core/Application.h"
#include "Engine/Rendering/RenderCommand.h"
#include "Engine/Rendering/VertexArray.h"

#include <bgfx\include\bgfx\bgfx.h>
#include <bx\include\bx\bx.h>

namespace Engine
{

	void EditorLayer::OnAttach()
	{
		Application& app = Application::Get();
		Window& window = app.GetWindow();

		m_EditorCamera = EditorCamera(90.0f, (float)window.GetWidth() / (float)window.GetHeight(), 0.01f, 10000.0f);
		m_EditorCamera.SetDistance(50.0f);
		
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		m_EditorCamera.OnUpdate(ts);

		RenderCommand::BeginFrame();

		Application& app = Application::Get();
		RenderCommand::SetViewport(0, 0, app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
		RenderCommand::SetCamera(m_EditorCamera.GetViewMatrix(), m_EditorCamera.GetProjectionMatrix());

		RenderCommand::DrawTestCubes(ts);

		RenderCommand::EndFrame();
	}

	void EditorLayer::OnImGuiRender()
	{

	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_EditorCamera.OnEvent(event);
	}
}



