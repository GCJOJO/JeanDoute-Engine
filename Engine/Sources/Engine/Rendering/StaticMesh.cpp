#include "enginepch.h"
#include "Engine/Utils/MathUtils.h"
#include "Engine/Rendering/StaticMesh.h"
#include "Engine/Rendering/RenderCommand.h"

#include "bx/math.h"
#include "glm/gtx/quaternion.hpp"

namespace Engine
{
	struct StaticMeshVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;

		static void Init()
		{
			ms_layout
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				//.add(bgfx::Attrib::Color1, )
				.end();
		}

		static bgfx::VertexLayout ms_layout;
	};

	bgfx::VertexLayout StaticMeshVertex::ms_layout;
	

	void StaticMesh::InitVertexLayout()
	{
		StaticMeshVertex::Init();
	}

	StaticMesh::StaticMesh(Ref<StaticMeshAsset> meshAsset, Ref<ShaderProgram> shaderProgram)
		: m_StaticMeshAsset(meshAsset), m_ShaderProgram(shaderProgram)
	{
		RebuildBuffers();
	}

	void StaticMesh::RebuildBuffers()
	{
		for (const auto& handles : m_MeshHandles)
		{
			if (bgfx::isValid(handles.VerticesHandle))
				bgfx::destroy(handles.VerticesHandle);

			if (bgfx::isValid(handles.IndicesHandle))
				bgfx::destroy(handles.IndicesHandle);
		}

		m_MeshHandles.clear();
		
		const std::vector<StaticMeshAsset::MeshData>& meshes = m_StaticMeshAsset->GetMeshes();

		for (const auto& mesh : meshes)
		{
			std::vector<StaticMeshVertex> vertices{ };
			vertices.reserve(mesh.vertices.size());
			for (int i = 0; i < mesh.vertices.size(); i++)
			{
				const auto& vertex = mesh.vertices[i];
				const auto& normals = mesh.normals[i];
				const auto& uvs = mesh.uvs[i];

				StaticMeshVertex newVertex{ };
				newVertex.x = vertex.x;
				newVertex.y = vertex.y;
				newVertex.z = vertex.z;

				newVertex.nx = normals.nx;
				newVertex.ny = normals.ny;
				newVertex.nz = normals.nz;

				newVertex.u = uvs.u;
				newVertex.v = uvs.v;

				vertices.push_back(newVertex);
			}

			auto vbh = bgfx::createVertexBuffer(
				bgfx::copy(vertices.data(), (uint32_t)(vertices.size() * sizeof(StaticMeshVertex))),
				StaticMeshVertex::ms_layout);

			auto ibh = bgfx::createIndexBuffer(
				bgfx::copy(mesh.indices.data(), (uint32_t)(mesh.indices.size() * sizeof(uint16_t))));
			
			m_MeshHandles.push_back({ vbh, ibh });
		}
	}

	void StaticMesh::Render(const bgfx::ViewId& viewId, glm::mat4& modelMatrix)
	{		
		for (const auto& handles : m_MeshHandles)
		{
			RenderCommand::DrawMesh(viewId, handles.VerticesHandle, handles.IndicesHandle, m_ShaderProgram->GetProgramHandle(), modelMatrix);
		}
	}

}