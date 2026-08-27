#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Window.h"

#include "bgfx/bgfx.h"

namespace Engine
{
	enum class RenderingAPI
	{
		None = 0,
		OpenGL,
		Vulkan,
		Direct3D11,
		Direct3D12
	};

	class Renderer
	{
	public:
		static void Init(RenderingAPI api, Window* window);
		static void Update();
		static void Shutdown();
		
		static void OnWindowResize(uint32_t width, uint32_t height);

		static const RenderingAPI GetRenderingAPI() { return m_RenderingAPI; }
		
		static bgfx::ViewId AllocateViewId();

	private:
		 static RenderingAPI m_RenderingAPI;
	};
}


