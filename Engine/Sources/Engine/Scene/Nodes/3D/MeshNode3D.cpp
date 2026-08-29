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
		SetMesh(staticMeshAsset, shaderProgram);
	}

	MeshNode3D::MeshNode3D(StringName name, Ref<StaticMeshAsset> staticMeshAsset, Ref<ShaderProgram> shaderProgram)
		: RenderableNode3D()
	{
		m_Name = name;
		SetMesh(staticMeshAsset, shaderProgram);
	}

	MeshNode3D::MeshNode3D()
	{

	}

	MeshNode3D::~MeshNode3D()
	{

	}

	void MeshNode3D::SetMesh(Ref<StaticMeshAsset> staticMeshAsset, Ref<ShaderProgram> shaderProgram)
	{
		m_StaticMesh = CreateRef<StaticMesh>(staticMeshAsset, shaderProgram);
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

	void MeshNode3D::SaveNode(YAML::Emitter& out) const
	{
		Node3D::SaveNode(out);
		out << YAML::Key << "Mesh" << YAML::BeginMap;
		out << YAML::Key << "Asset" << YAML::Value << m_StaticMesh->GetStaticMeshAsset()->GetAssetHandle();
		// Save Shader or Material (TODO Material System)

		out << YAML::EndMap;
	}

	void MeshNode3D::LoadNode(const YAML::Node& data)
	{
		Node3D::LoadNode(data);
		const auto& meshData = data["Mesh"];
		if (meshData)
		{
			AssetHandle meshHandle = data["Asset"].as<uint64_t>();
			__debugbreak(); // To check mesh handle value
		}
	}

}