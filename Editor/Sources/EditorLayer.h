#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Core/Layer.h"
#include "Engine/Scene/Nodes/3D/MeshNode3D.h"
#include "Engine/Rendering/Shader.h"
#include "Engine/Rendering/StaticMesh.h"
#include "Engine/Assets/StaticMeshAsset.h"
#include "Engine/Rendering/Texture.h"
#include "Engine/Rendering/Framebuffer.h"
#include "Editor/Panels/SceneHierarchyPanel.h"
#include "EditorCamera.h"

#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/ApplicationEvent.h"

namespace Engine
{
	class EditorLayer : public Layer 
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDetach() override;


		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;


		void OnEvent(Event& event) override;

		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnWindowDrop(WindowDropEvent& e);
		//void OnOverlayRender();
	private:
		void NewProject();
		bool OpenProject();
		bool OpenProject(const std::filesystem::path& path);
		void SaveProject();

		void NewScene();
		void OpenScene();
		void OpenScene(AssetHandle handle);
		void SaveScene();
		void SaveSceneAs();
		
		void ImportAssets(const std::filesystem::path& directory);

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

	private:
		std::filesystem::path m_EditorScenePath;
		SceneHierarchyPanel m_SceneHierarchyPanel;

		EditorCamera m_EditorCamera;
		Ref<Shader> m_VertexShader;
		Ref<Shader> m_FragmentShader;
		Ref<ShaderProgram> m_ShaderProgram;
		Ref<Texture2D> m_BobTexture;
		Ref<StaticMeshAsset> m_MonkeyMeshAsset;
		MeshNode3D* m_MonkeyMesh;

		bgfx::UniformHandle m_TextureUniform;

		Ref<Framebuffer> m_Framebuffer;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
	};
}