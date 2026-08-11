#pragma once
#include "VertexArray.h"

namespace Engine
{
	class RenderCommand
	{
	public:

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void SetWindowSize(uint32_t width, uint32_t height);

		static void SetClearColor(const glm::vec4& color);
		static void BeginFrame();
		static void EndFrame();
		
		static void DrawIndexed(Ref<VertexArray>& vertexArray, uint32_t vertexCount);
		//static void DrawLines();
		//static void SetLinesWidth(float width);
		
	};
}