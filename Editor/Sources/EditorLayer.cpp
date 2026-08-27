#include "EditorLayer.h"
#include "Engine/Core/Application.h"
#include "Engine/Rendering/RenderCommand.h"
#include "Engine/Rendering/Font.h"
#include "Engine/Assets/AssetImporter.h"
#include "Engine/Assets/TextureImporter.h"
#include "Engine/Scene/SceneTree.h"
#include "Editor/Scene/EditorSceneManager.h"
#include "Engine/Utils/MathUtils.h"

#include <bgfx/include/bgfx/bgfx.h>
#include <bx/include/bx/bx.h>

#include "imgui/imgui.h"

namespace Engine
{
	static Ref<Font> s_Font;
	static float s_Time = 0.0f;

	EditorLayer::EditorLayer() : Layer("Editor")
	{
		s_Font = Font::GetDefault();

		SceneManager::Initialize(CreateRef<EditorSceneManager>());
		SceneManager::NewScene();

		m_SceneHierarchyPanel = SceneHierarchyPanel();
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
				/*if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
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

				if (ImGui::MenuItem("Export Project..."))
					ExportProject();*/

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
	}
}



