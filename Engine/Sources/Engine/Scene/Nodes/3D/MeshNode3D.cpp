#include "enginepch.h"
#include "Engine/Scene/Nodes/3D/MeshNode3D.h"
#include "Engine/Assets/StaticMeshAsset.h"

#include "Engine/Rendering/StaticMesh.h"
#include "Engine/Rendering/Shader.h"

namespace Engine
{

	MeshNode3D::MeshNode3D(Ref<StaticMeshAsset> staticMeshAsset, Ref<ShaderProgram> shaderProgram)
		: RenderableNode3D()
	{
		m_Name = "Mesh Node 3D";
		m_StaticMesh = CreateRef<StaticMesh>(staticMeshAsset, shaderProgram);
	}

	MeshNode3D::MeshNode3D(StringName name, Ref<StaticMeshAsset> staticMeshAsset, Ref<ShaderProgram> shaderProgram)
		: RenderableNode3D()
	{
		m_Name = name;
		m_StaticMesh = CreateRef<StaticMesh>(staticMeshAsset, shaderProgram);
		Renderable3DRegistry::Get().Register(this);
	}

	MeshNode3D::~MeshNode3D()
	{

	}

	void MeshNode3D::Render(const bgfx::ViewId& viewId)
	{
		if (!m_StaticMesh)
			return;

		glm::mat4 meshTransforms = m_Transform.ToMatrix();
		m_StaticMesh->Render(viewId, meshTransforms);
	}

	void MeshNode3D::OnReady()
	{

	}

	void MeshNode3D::OnUpdate(float dt)
	{

	}

	void MeshNode3D::OnDestroy()
	{

	}

}