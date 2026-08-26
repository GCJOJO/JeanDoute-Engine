#include "enginepch.h"
#include "Engine/Utils/MathUtils.h"
#include "Engine/Rendering/StaticMesh.h"
#include "Engine/Rendering/RenderCommand.h"

#include "bx/math.h"
#include "glm/gtx/quaternion.hpp"

namespace Engine
{
	struct PosNormalUvVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;

		static void init()
		{
			ms_layout
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();
		}

		static bgfx::VertexLayout ms_layout;
	};

	static bool s_LayoutInit = false;
	bgfx::VertexLayout PosNormalUvVertex::ms_layout;
	

	StaticMesh::StaticMesh(Ref<StaticMeshAsset> meshAsset, Ref<ShaderProgram> shaderProgram)
		: m_StaticMeshAsset(meshAsset), m_ShaderProgram(shaderProgram)
	{
		// Maybe find a better way
		if(!s_LayoutInit)
		{
			PosNormalUvVertex::init();
			s_LayoutInit = true;
		}

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
			std::vector<PosNormalUvVertex> vertices{ };
			vertices.reserve(mesh.vertices.size());
			for (int i = 0; i < mesh.vertices.size(); i++)
			{
				const auto& vertex = mesh.vertices[i];
				const auto& normals = mesh.normals[i];
				const auto& uvs = mesh.uvs[i];

				PosNormalUvVertex newVertex{ };
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
				bgfx::copy(vertices.data(), (uint32_t)(vertices.size() * sizeof(PosNormalUvVertex))),
				PosNormalUvVertex::ms_layout);

			auto ibh = bgfx::createIndexBuffer(
				bgfx::copy(mesh.indices.data(), (uint32_t)(mesh.indices.size() * sizeof(uint16_t))));
			
			m_MeshHandles.push_back({ vbh, ibh });
		}
	}

	void StaticMesh::Render(bgfx::ViewId viewId)
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

		glm::mat4 meshTransforms = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) *
			glm::toMat4(glm::quat(glm::vec3(0, glm::radians(180.0f), 0))) *
			glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
		
		for (const auto& handles : m_MeshHandles)
		{
			RenderCommand::DrawMesh(viewId, handles.VerticesHandle, handles.IndicesHandle, m_ShaderProgram->GetProgramHandle(), meshTransforms);
		}
	}

}