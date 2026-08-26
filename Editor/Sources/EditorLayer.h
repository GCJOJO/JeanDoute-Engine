#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Core/Layer.h"
#include "Engine/Rendering/Shader.h"
#include "Engine/Rendering/StaticMesh.h"
#include "Engine/Assets/StaticMeshAsset.h"
#include "EditorCamera.h"

namespace Engine
{
	class EditorLayer : public Layer 
	{
	public:
		void OnAttach() override;
		void OnDetach() override;


		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;


		void OnEvent(Event& event) override;
	private:
		EditorCamera m_EditorCamera;
		Ref<Shader> m_VertexShader;
		Ref<Shader> m_FragmentShader;
		Ref<ShaderProgram> m_ShaderProgram;
		Ref<StaticMeshAsset> m_MonkeyMeshAsset;
		Ref<StaticMesh> m_MonkeyMesh;
	};
}