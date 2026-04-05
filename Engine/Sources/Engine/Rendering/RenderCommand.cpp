#include "enginepch.h"
#include "Engine/Rendering/RenderCommand.h"
#include <bgfx\bgfx.h>

namespace Engine
{

	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		bgfx::setViewRect(0, x, y, uint16_t(width), uint16_t(height));
	}

	void RenderCommand::SetWindowSize(uint32_t width, uint32_t height)
	{
		bgfx::reset(width, height, BGFX_RESET_VSYNC, bgfx::TextureFormat::RGBA8);
	}

	void RenderCommand::SetClearColor(const glm::vec4& color)
	{
		uint32_t bgfxColor =
			(uint8_t(color.r * 255.0f) << 24) |
			(uint8_t(color.g * 255.0f) << 16) |
			(uint8_t(color.b * 255.0f) << 8) |
			(uint8_t(color.a * 255.0f));

		bgfx::setViewClear(0, BGFX_CLEAR_COLOR, bgfxColor, 1.0, 0);
	}

	void RenderCommand::BeginFrame()
	{
		bgfx::touch(0);
	}

	void RenderCommand::EndFrame()
	{
		bgfx::frame();
	}

}