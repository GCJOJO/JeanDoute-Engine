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

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void SetWindowSize(uint32_t width, uint32_t height);

		static void SetClearColor(const glm::vec4& color);
		static void BeginFrame();
		static void EndFrame();
		
		static void SetCamera(glm::mat4 cameraViewMatrix, glm::mat4 cameraProjectionMatrix);
		static void DrawTestCubes(Timestep ts, Ref<ShaderProgram> shaderProgram);
		static void DrawMesh(bgfx::ViewId view, bgfx::VertexBufferHandle vbh, bgfx::IndexBufferHandle ibh, bgfx::ProgramHandle shaderProgram, const glm::mat4& transforms = glm::identity<glm::mat4>());
		//static void DrawLines();
		//static void SetLinesWidth(float width);
		
	};
}