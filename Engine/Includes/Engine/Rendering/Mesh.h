#pragma once
#include "bgfx/bgfx.h"

namespace Engine
{
	class ShaderProgram;

	class Mesh
	{
	public:
		virtual const bgfx::VertexBufferHandle& GetVertexBufferHandle(int meshId) = 0;
		virtual const bgfx::IndexBufferHandle& GetIndexBufferHandle(int meshId) = 0;
		virtual const Ref<ShaderProgram> GetShaderProgram() = 0;

		virtual int GetMeshCount() = 0;
	};
}