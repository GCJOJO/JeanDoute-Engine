#include "EditorLayer.h"
#include "Engine/Core/Application.h"
#include "Engine/Rendering/RenderCommand.h"
#include "Engine/Rendering/Font.h"
#include "Engine/Assets/AssetImporter.h"
#include "Engine/Assets/TextureImporter.h"
#include "Engine/Scene/SceneTree.h"
#include "Editor/Scene/EditorSceneManager.h"
#include "Engine/Utils/MathUtils.h"
#include "Engine/ScriptEngine/ScriptEngine.h"
#include "Engine/Utils/PlatformUtils.h"
#include "Engine/Project/Project.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/SceneImporter.h"
#include "Editor/EditorConfig.h"
#include "Engine/Assets/EditorAssetManager.h"

#include "Engine/Core/Input.h"

#include <bgfx/include/bgfx/bgfx.h>

#include "imgui/imgui.h"

namespace Engine
{
	static Ref<Font> s_Font;
	static float s_Time = 0.0f;

	static std::filesystem::path s_CoreAssemblyPath = Filesystem::GetExecutableDir() / "CSharp-Core.dll";

	EditorLayer::EditorLayer() : Layer("Editor")
	{
		s_Font = Font::GetDefault();

		SceneManager::Initialize(CreateRef<EditorSceneManager>());
		SceneManager::NewScene();

		m_SceneHierarchyPanel = SceneHierarchyPanel();

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			OpenProject(projectFilePath);
		}
		else
		{
			// TODO : Prompt user to select a directory
			//NewProject();

			// If no project is opened, close Editor
			// NOTE: this is while we don't have a new project path

			EditorConfig::Deserialize();

			if (EditorConfig::LastProjectPath.has_extension() && EditorConfig::LastProjectPath.extension() == ".eproj")
			{
				if (OpenProject(EditorConfig::LastProjectPath))
					return;
			}

			if (!OpenProject())
				Application::Get().Close();
		}
	}

	void EditorLayer::OnAttach()
	{
		Application& app = Application::Get();
		Window& window = app.GetWindow();

		m_EditorCamera = EditorCamera(90.0f, (float)window.GetWidth() / (float)window.GetHeight(), 0.01f, 10000.0f);
		m_EditorCamera.SetDistance(50.0f);

		std::filesystem::path vertexShaderPath = "monkey.vert";
		std::filesystem::path fragmentShaderPath = "monkey.frag";
		std::filesystem::path texturePath = "Assets/Textures/Bob.jpg";

		m_VertexShader = CreateRef<Shader>(Shader::Type::Vertex, vertexShaderPath);
		m_FragmentShader = CreateRef<Shader>(Shader::Type::Fragment, fragmentShaderPath);
		m_ShaderProgram = CreateRef<ShaderProgram>(m_VertexShader, m_FragmentShader);
		m_BobTexture = TextureImporter::ImportTexture2D(texturePath);

		m_MonkeyMeshAsset = std::static_pointer_cast<StaticMeshAsset>(AssetImporter::ImportAsset(Asset::Type::StaticMesh, "Assets/Meshes/Monkey.glb"));
		
		m_TextureUniform = bgfx::createUniform("u_texture", bgfx::UniformType::Sampler);

		SceneTree& sceneTree = SceneTree::Get();
		sceneTree.ClearSceneTree();

		Node* newNode = sceneTree.GetRoot()->AddChild(CreateScope<Node>("NewNode"));
		m_MonkeyMesh = newNode->CreateChild<MeshNode3D>(m_MonkeyMeshAsset, m_ShaderProgram);

		for (int i = 0; i < 25; i++)
		{
			newNode->AddChild(CreateScope<Node>("ChildNode"));
		}

		for (const auto& child : sceneTree.GetRoot()->GetChildren(true))
		{
			ENGINE_CORE_INFO(child->GetName());
		}

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = window.GetWidth();
		fbSpec.Height = window.GetHeight();

		m_Framebuffer = CreateRef<Framebuffer>(fbSpec);

		m_SceneHierarchyPanel.SetContext(SceneManager::GetCurrentScene());

		m_MonkeyMesh->SetPosition(Vec3(0, 0, 0));
		m_MonkeyMesh->SetRotation(Vec3(0, 180, 0));
		m_MonkeyMesh->SetScale(10);
	}

	void EditorLayer::OnDetach()
	{
		if (bgfx::isValid(m_TextureUniform))
			bgfx::destroy(m_TextureUniform);
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		s_Time += ts.GetSeconds();

		m_EditorCamera.OnUpdate(ts);

		RenderCommand::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), m_Framebuffer->GetViewId());
		RenderCommand::BeginFrame(m_Framebuffer->GetViewId());
	
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}

		m_Framebuffer->Bind();
		m_Framebuffer->ClearAttachment(1, -1);

		Application& app = Application::Get();
		RenderCommand::SetViewport(m_Framebuffer->GetViewId(), 0, 0, app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
		RenderCommand::SetCamera(m_Framebuffer->GetViewId(), m_EditorCamera.GetViewMatrix(), m_EditorCamera.GetProjectionMatrix());

		//RenderCommand::DrawTestCubes(ts, m_ShaderProgram);
		m_BobTexture->Bind(m_TextureUniform);

		m_MonkeyMesh->GetTransform().position = 
			(Vec3::UP * MathUtils::sin(s_Time * 100) * 10) + 
			(Vec3::RIGHT * MathUtils::cos(s_Time * 25) * 5) +
			(Vec3::FORWARD * MathUtils::tan(s_Time * 10) * 7.5f);

		SceneManager::GetCurrentScene()->UpdateScene(ts, m_Framebuffer->GetViewId());

		RenderCommand::EndFrame();

		//m_Framebuffer->Unbind();

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

		my = viewportSize.y - my;

		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			//m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, SceneManager::GetCurrentScene().get());
			//ENGINE_CORE_INFO("Hovered Entity ID : {}", pixelData);
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockingEnabled = true;
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
					OpenProject();

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();

				//if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
				//	OpenScene();

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				//if (ImGui::MenuItem("Export Project..."))
				//	ExportProject();

				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scripts"))
			{
				/*if (ImGui::MenuItem("Reload Assembly", "Ctrl+R"))
					ScriptEngine::ReloadAssembly();*/
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Stats");

		const bgfx::Stats* stats = bgfx::getStats();

		int frameBegin = stats->cpuTimeBegin;
		int frameEnd = stats->cpuTimeEnd;
		int frameTime = frameEnd - frameBegin;
		int freq = stats->cpuTimerFreq;

		double fps = (double)freq / (double)frameTime;

		ImGui::Text("FPS %6.2f", fps);

		ImGui::Image((ImTextureID)s_Font->GetAtlasTexture()->GetTextureHandle().idx, { 512, 512 }, { 0, 1 }, { 1, 0 });

#if 0
		std::string name = "None";
		if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;

		ImGui::Text("Hovered Entity : %s", name.c_str());
#endif

		ImGui::End();

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *(glm::vec2*)&viewportPanelSize && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
		{
			m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			m_EditorCamera.SetViewportSize(viewportPanelSize.x, viewportPanelSize.y);
			//SceneManager::GetCurrentScene()->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0).idx;
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 0 }, ImVec2{ 1, 1 });

		ImGui::End(); // Viewport
		ImGui::PopStyleVar(ImGuiStyleVar_WindowPadding);

		ImGui::End(); // Dockspace Demo
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_EditorCamera.OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(ENGINE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(ENGINE_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
		dispatcher.Dispatch<WindowDropEvent>(ENGINE_BIND_EVENT_FN(EditorLayer::OnWindowDrop));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.IsRepeat()) return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
		case (int)Key::N:
			if (control)
				NewScene();
			break;
		case (int)Key::O:
			if (control)
				OpenProject();
			break;
		case (int)Key::S:
			if (control)
			{
				if (shift)
					SaveSceneAs();
				else
					SaveScene();
			}
			break;

			// Scene Commands
		/*case (int)Key::D:
			if (control)
				OnDuplicateEntity();
			break;

			// Gizmos
		case (int)Key::Q:
			m_GizmoType = -1;
			break;
		case (int)Key::W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case (int)Key::E:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case (int)Key::R:
			if (control)
				ScriptEngine::ReloadAssembly();
			else
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;*/

		/*case (int)Key::Delete:
		{
			if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() != 0)
				break;

			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			if (m_HoveredEntity)
			{
				m_SceneHierarchyPanel.SetSelectedEntity({});
				SceneManager::GetCurrentScene()->DestroyEntity(m_HoveredEntity);
			}
			break;
		}*/
		}
		return true;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		/*if (e.GetMouseButton() == (int)MouseCode::ButtonLeft && m_ViewportHovered &&
			!ImGuizmo::IsOver() && !Input::IsKeyPressed(KeyCode::LeftAlt))
		{
			m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			return true;
		}*/
		return false;
	}

	bool EditorLayer::OnWindowDrop(WindowDropEvent& e)
	{
		return false;
	}

	void EditorLayer::NewProject()
	{
		Project::New();
	}

	bool EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		if (auto project = Project::Load(path))
		{
			EditorConfig::LastProjectPath = path;
			EditorConfig::Serialize();

			ScriptEngine::Init(s_CoreAssemblyPath.string());
			//ScriptEngine::SetStopFunction([this]() { OnSceneStop();	});

			AssetHandle startScene = Project::GetActive()->GetConfig().StartScene;
			ImportAssets(Project::GetActiveAssetDirectory());
			if (!startScene)
				NewScene();
			else
				OpenScene(startScene);
			//m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>(project);
			return true;
		}
		return false;
	}

	bool EditorLayer::OpenProject()
	{
		std::string filepath = FileDialogs::OpenFile("Engine Project (*.eproj)\0*.eproj\0");
		if (!filepath.empty())
			return OpenProject(filepath);
		return false;
	}

	void EditorLayer::SaveProject()
	{
		//Project::SaveActive();
	}

	void EditorLayer::NewScene()
	{
		SceneManager::NewScene();
		//m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(SceneManager::GetCurrentScene());

		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		// 		std::string filepath = FileDialogs::OpenFile("Engine Scene (*.scene)\0*.scene\0");
		// 		if (!filepath.empty())
		// 			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(AssetHandle handle)
	{
		ENGINE_CORE_ASSERT(handle);

		/*if (m_SceneState != SceneState::Edit)
			OnSceneStop();*/

		SceneManager::OpenScene(handle);

		Ref<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(handle);
		Ref<Scene> newScene = Scene::Copy(readOnlyScene);

		EditorSceneManager* editorSceneManager = (EditorSceneManager*)SceneManager::Get().get();
		m_SceneHierarchyPanel.SetContext(editorSceneManager->GetEditorScene());

		m_EditorScenePath = Project::GetActive()->GetEditorAssetManager()->GetFilePath(handle);

	}

	void EditorLayer::SaveScene()
	{
		EditorSceneManager* editorSceneManager = (EditorSceneManager*)SceneManager::Get().get();
		if (!m_EditorScenePath.empty())
			SerializeScene(editorSceneManager->GetEditorScene(), m_EditorScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		EditorSceneManager* editorSceneManager = (EditorSceneManager*)SceneManager::Get().get();
		std::string filepath = FileDialogs::SaveFile("Engine Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			SerializeScene(editorSceneManager->GetEditorScene(), filepath);
			m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::ImportAssets(const std::filesystem::path& directory)
	{
		Ref<EditorAssetManager> assetManager = std::static_pointer_cast<EditorAssetManager>(Project::GetActive()->GetAssetManager());
		for (const auto& file : std::filesystem::directory_iterator(directory))
		{
			const std::filesystem::path& filepath = directory / file;
			ENGINE_CORE_INFO("Iterating over {}", filepath.string());
			if (std::filesystem::is_directory(filepath))
			{
				ImportAssets(filepath);
				continue;
			}


			assetManager->ImportAsset(filepath);
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneImporter::SaveScene(scene, path);
	}
}



