#pragma once
#include "Engine/Assets/StaticMeshAsset.h"
#include "Engine/Rendering/Shader.h"

#include "bgfx/bgfx.h"

namespace Engine
{
	class StaticMesh
	{
	public:
		static void InitVertexLayout();
		
		StaticMesh(Ref<StaticMeshAsset> meshAsset, Ref<ShaderProgram> shaderProgram);

		void RebuildBuffers();
		void Render(const bgfx::ViewId& viewId, glm::mat4& modelMatrix);
	private:
		struct MeshHandles
		{
			bgfx::VertexBufferHandle VerticesHandle;
			bgfx::IndexBufferHandle  IndicesHandle;
		};

		Ref<StaticMeshAsset> m_StaticMeshAsset;
		Ref<ShaderProgram> m_ShaderProgram;

		std::vector<MeshHandles> m_MeshHandles{ };
	};
}