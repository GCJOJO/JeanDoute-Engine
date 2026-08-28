#pragma once
#include "Engine/Rendering/Mesh.h"

namespace Engine
{
	using ViewId = uint16_t;

	class RenderCommand
	{
	public:
		static void Init();

		static void SetViewport(const ViewId& viewId, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void SetWindowSize(uint32_t width, uint32_t height);

		static void SetClearColor(const glm::vec4& color, const ViewId& viewId = 0);
		static void BeginFrame(const ViewId& viewId = 0);
		static void EndFrame();
		
		static void SetCamera(const ViewId& viewId, glm::mat4 cameraViewMatrix, glm::mat4 cameraProjectionMatrix);
		static void DrawMesh(const ViewId& view, Mesh& mesh, const glm::mat4& transforms = glm::identity<glm::mat4>());
		//static void DrawLines();
		//static void SetLinesWidth(float width);
		
	};
}