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
		
		StaticMesh(SharedPtr<StaticMeshAsset> meshAsset, SharedPtr<ShaderProgram> shaderProgram);

		void RebuildBuffers();
		void Render(const bgfx::ViewId& viewId, glm::mat4& modelMatrix);

		virtual const bgfx::VertexBufferHandle& GetVertexBufferHandle(int meshId) override { return m_MeshHandles[meshId].VerticesHandle; }
		virtual const bgfx::IndexBufferHandle& GetIndexBufferHandle(int meshId) override { return m_MeshHandles[meshId].IndicesHandle; }
		virtual const SharedPtr<ShaderProgram> GetShaderProgram() override { return m_ShaderProgram; }
		SharedPtr<StaticMeshAsset> GetStaticMeshAsset() const { return m_StaticMeshAsset; }
		
		virtual int GetMeshCount() override { return m_MeshHandles.size(); }

	private:
		struct MeshHandles
		{
			bgfx::VertexBufferHandle VerticesHandle;
			bgfx::IndexBufferHandle  IndicesHandle;
		};

		SharedPtr<StaticMeshAsset> m_StaticMeshAsset;
		SharedPtr<ShaderProgram> m_ShaderProgram;

		std::vector<MeshHandles> m_MeshHandles{ };
	};
}