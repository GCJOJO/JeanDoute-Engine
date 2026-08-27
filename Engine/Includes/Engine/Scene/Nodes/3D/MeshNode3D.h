#pragma once
#include "Engine/Scene/Nodes/3D/RenderableNode3D.h"
#include "bgfx/bgfx.h"

namespace Engine
{
	class StaticMesh;
	class StaticMeshAsset;
	class ShaderProgram;

	class MeshNode3D : public RenderableNode3D
	{
	public:
		static void InitVertexLayout();

		MeshNode3D(Ref<StaticMeshAsset> staticMeshAsset, Ref<ShaderProgram> shaderProgram);
		MeshNode3D(StringName name, Ref<StaticMeshAsset> staticMeshAsset, Ref<ShaderProgram> shaderProgram);
		virtual ~MeshNode3D() override;
		
		virtual void Render(const bgfx::ViewId& viewId) override;

		virtual void OnReady() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

	private:
		Ref<StaticMesh> m_StaticMesh;
	};
}