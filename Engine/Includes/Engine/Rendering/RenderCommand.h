#pragma once
#include "Engine/Core/Timestep.h"
#include "Engine/Core/Core.h"
#include "Engine/Rendering/Shader.h"

namespace Engine
{
	class RenderCommand
	{
	public:
		static void Init();

		static void SetViewport(bgfx::ViewId viewId, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void SetWindowSize(uint32_t width, uint32_t height);

		static void SetClearColor(const glm::vec4& color, const bgfx::ViewId& viewId = 0);
		static void BeginFrame(const bgfx::ViewId& viewId = 0);
		static void EndFrame();
		
		static void SetCamera(const bgfx::ViewId& viewId, glm::mat4 cameraViewMatrix, glm::mat4 cameraProjectionMatrix);
		static void DrawTestCubes(Timestep ts, Ref<ShaderProgram> shaderProgram);
		static void DrawMesh(bgfx::ViewId view, bgfx::VertexBufferHandle vbh, bgfx::IndexBufferHandle ibh, bgfx::ProgramHandle shaderProgram, const glm::mat4& transforms = glm::identity<glm::mat4>());
		//static void DrawLines();
		//static void SetLinesWidth(float width);
		
	};
}