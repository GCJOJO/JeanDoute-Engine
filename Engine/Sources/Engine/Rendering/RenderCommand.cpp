#include "enginepch.h"
#include "Engine/Rendering/RenderCommand.h"
#include "Engine/Rendering/Shader.h"

#include <bgfx/bgfx.h>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	
	void RenderCommand::SetViewport(const ViewId& viewId, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		bgfx::setViewRect(viewId, x, y, uint16_t(width), uint16_t(height));
	}

	void RenderCommand::SetWindowSize(uint32_t width, uint32_t height)
	{
		bgfx::reset(width, height, BGFX_RESET_VSYNC | BGFX_RESET_HIDPI, bgfx::TextureFormat::RGBA8);
	}

	void RenderCommand::SetClearColor(const glm::vec4& color, const bgfx::ViewId& viewId)
	{
		uint32_t bgfxColor =
			(uint8_t(color.r * 255.0f) << 24) |
			(uint8_t(color.g * 255.0f) << 16) |
			(uint8_t(color.b * 255.0f) << 8) |
			(uint8_t(color.a * 255.0f));

		bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, bgfxColor, 1.0f, 0);
	}

	void RenderCommand::BeginFrame(const ViewId& viewId)
	{
		bgfx::touch(viewId);
	}

	void RenderCommand::EndFrame()
	{
		bgfx::frame();
	}

	void RenderCommand::SetCamera(const ViewId& viewId, glm::mat4 cameraViewMatrix, glm::mat4 cameraProjectionMatrix)
	{
		float* view = glm::value_ptr(cameraViewMatrix);
		float* proj = glm::value_ptr(cameraProjectionMatrix);
		bgfx::setViewTransform(viewId, view, proj);
	}

	void RenderCommand::DrawMesh(const ViewId& view, Mesh& mesh, const glm::mat4& transforms)
	{
		uint64_t state = 0
			| BGFX_STATE_WRITE_R
			| BGFX_STATE_WRITE_G
			| BGFX_STATE_WRITE_B
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CCW
			| BGFX_STATE_MSAA
			| BGFX_STATE_PT_TRISTRIP;

		bgfx::setTransform(glm::value_ptr(transforms));

		for (int i = 0; i < mesh.GetMeshCount(); i++)
		{
			bgfx::setVertexBuffer(0, mesh.GetVertexBufferHandle(i));
			bgfx::setIndexBuffer(mesh.GetIndexBufferHandle(i));

			bgfx::submit(view, mesh.GetShaderProgram()->GetProgramHandle());
		}

	}

}