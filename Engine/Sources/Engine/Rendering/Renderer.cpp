#include "enginepch.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/RenderCommand.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/StaticMesh.h"

#include "bgfx/bgfx.h"

#ifdef ENGINE_PLATFORM_WINDOWS	
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Engine
{
	const bgfx::ViewId BaseViewId{ 0 };
	static bgfx::ViewId s_ViewId = 1;

	RenderingAPI Renderer::m_RenderingAPI = RenderingAPI::None;

	void Renderer::Init(RenderingAPI api, Window* window)
	{
		m_RenderingAPI = api;

		bgfx::Init init = bgfx::Init();
		
		switch (m_RenderingAPI)
		{
		case RenderingAPI::OpenGL:
			init.type = bgfx::RendererType::OpenGL;
			break;
		case RenderingAPI::Vulkan:
			init.type = bgfx::RendererType::Vulkan;
			break;
		case RenderingAPI::Direct3D11:
			init.type = bgfx::RendererType::Direct3D11;
			break;
		case RenderingAPI::Direct3D12:
			init.type = bgfx::RendererType::Direct3D12;
			break;
		}

		init.vendorId = BGFX_PCI_ID_NONE;
		
#	if ENGINE_PLATFORM_LINUX
		if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND)
			init.platformData.nwh = glfwGetWaylandWindow(window->GetNativeWindow());
		else
			init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window->GetNativeWindow());
#	elif ENGINE_PLATFORM_MACOS
		init.platformData.nwh = glfwGetCocoaWindow(window->GetNativeWindow());
#	elif ENGINE_PLATFORM_WINDOWS
		init.platformData.nwh = glfwGetWin32Window((GLFWwindow*)window->GetNativeWindow());
#	endif

		init.platformData.ndt = nullptr;
		init.platformData.type = bgfx::NativeWindowHandleType::Default;
		init.resolution.width = window->GetWidth();
		init.resolution.height = window->GetHeight();

		init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_HIDPI;

		bgfx::init(init);

		//bgfx::setDebug(BGFX_DEBUG_STATS | BGFX_DEBUG_TEXT | BGFX_DEBUG_PROFILER);
		ShaderManager::CompileShaders("Shaders");

		bgfx::setViewRect(BaseViewId, 0, 0, init.resolution.width, init.resolution.height);
		bgfx::setViewClear(BaseViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

		RenderCommand::Init();
		RenderCommand::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));


		StaticMesh::InitVertexLayout();
	}

	void Renderer::Update()
	{
	}

	void Renderer::Shutdown()
	{
		bgfx::shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetWindowSize(width, height);
		RenderCommand::SetViewport(BaseViewId, 0, 0, width, height);
	}

	bgfx::ViewId Renderer::AllocateViewId()
	{
		return s_ViewId++;
	}

}