#pragma once
#include "Engine/Assets/StaticMeshAsset.h"
#include "Engine/Rendering/Shader.h"
#include "Mesh.h"

#include "bgfx/bgfx.h"

namespace Engine
{
	class StaticMesh : public Mesh
	{
	public:
		static void InitVertexLayout();
		
		StaticMesh(Ref<StaticMeshAsset> meshAsset, Ref<ShaderProgram> shaderProgram);

		void RebuildBuffers();
		void Render(const bgfx::ViewId& viewId, glm::mat4& modelMatrix);

		virtual const bgfx::VertexBufferHandle& GetVertexBufferHandle(int meshId) override { return m_MeshHandles[meshId].VerticesHandle; }
		virtual const bgfx::IndexBufferHandle& GetIndexBufferHandle(int meshId) override { return m_MeshHandles[meshId].IndicesHandle; }
		virtual const Ref<ShaderProgram> GetShaderProgram() override { return m_ShaderProgram; }
		Ref<StaticMeshAsset> GetStaticMeshAsset() const { return m_StaticMeshAsset; }
		
		virtual int GetMeshCount() override { return m_MeshHandles.size(); }

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