#pragma once
#include "Engine/Scene/Nodes/3D/RenderableNode3D.h"
#include "bgfx/bgfx.h"

namespace Engine
{
	class StaticMesh;
	class StaticMeshAsset;
	class ShaderProgram;

	class MeshNode3D : 
		public RenderableNode3D,
		public AutoRegister<MeshNode3D, MeshNode3D, RenderableNode3D, Node3D, Node>
	{
		REGISTER_NODE_CLASS(MeshNode3D);
	public:
		static void InitVertexLayout();

		MeshNode3D();
		MeshNode3D(SharedPtr<StaticMeshAsset> staticMeshAsset, SharedPtr<ShaderProgram> shaderProgram);
		MeshNode3D(StringName name, SharedPtr<StaticMeshAsset> staticMeshAsset, SharedPtr<ShaderProgram> shaderProgram);
		virtual ~MeshNode3D() override;
		
		void SetMesh(SharedPtr<StaticMeshAsset> staticMeshAsset, SharedPtr<ShaderProgram> shaderProgram);

		virtual void Render(const bgfx::ViewId& viewId) override;

		virtual void OnReady() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

		virtual void SaveNode(YAML::Emitter& out) const override;
		virtual void LoadNode(const YAML::Node& data) override;

	private:
		SharedPtr<StaticMesh> m_StaticMesh;
	};
}