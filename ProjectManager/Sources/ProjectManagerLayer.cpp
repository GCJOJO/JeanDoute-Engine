#include "ProjectManagerLayer.h"
#include "Config.h"
#include "Engine/Core/Application.h"
#include "Engine/Project/Project.h"
#include "Engine/Utils/PlatformUtils.h"
#include "Engine/Rendering/RenderCommand.h"

#include <imgui/imgui.h>

#include <filesystem>
#include <format>
#include <string>
#include <regex>


namespace Engine
{

	//std::string premakeProjectFile = "local EngineRootDir = os.getenv(\"GAMINGENGINE_SOURCEDIR\")\nprint(EngineRootDir)\nworkspace \"{0}\"\narchitecture \"x64\"\nstartproject \"{0}\"\nconfigurations\n{\n\"Debug\",\n\"Release\",\n\"Dist\"\n }\nsolution_items{\".editorconfig\"}\nflags{\"MultiProcessorCompile\"}\noutputdir = \"%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\"\nproject \"{0}\"\nkind \"SharedLib\"\n\nlanguage \"C#\"\ndotnetframework \"4.7.2\"\ntargetdir (\"Binaries\")\nobjdir (\"Intermediates\")\nfiles\n{\n\"Source/**.cs\",\n\"Properties/**.cs\"\n}\nlinks{\"GamingEngine-ScriptCore\"}\nfilter \"configurations:Debug\"\noptimize \"Off\"\nsymbols \"Default\"\nfilter \"configurations:Release\"\noptimize \"On\"\nsymbols \"Default\"\nfilter \"configurations:Dist\"\noptimize \"Full\"\nsymbols \"Off\"\n\ngroup \"Engine\"\ninclude (EngineRootDir .. \"/GamingEngine-ScriptCore\")\ngroup \"\"";
	std::string premakeProjectFile = R"(local EngineRootDir = os.getenv("JEANDOUTEENGINE_DIR")
print(EngineRootDir)
include (EngineRootDir .. "/vendor/premake/premake_customization/solution_items.lua")

workspace "ProjectName"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

	solution_items
	{
		".editorconfig"
	}

    flags
    {
        "MultiProcessorCompile"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	csharp_outputdir = "%{cfg.buildcfg}-%{cfg.system}"

project "ProjectName"
	kind "SharedLib"
	language "C#"
	dotnetframework "net9.0"

	targetdir ("Binaries")
	objdir ("Intermediates")

	files
	{
		"Source/**.cs",
		"Properties/**.cs"
	}

    links
    {
        "CSharp-Core",
		"Coral.Managed"
    }
	
	vsprops {
		EnableDynamicLoading = "true"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"
	
	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"

group "Engine"
    include (EngineRootDir .. "/CSharp-Core")
	include (EngineRootDir .. "/Engine/vendor/coral/Coral.Managed")
group "")";

	using namespace std::literals;

	namespace Utils
	{
		std::filesystem::path ResolvePremakePath()
		{
			std::filesystem::path basePath = Filesystem::GetUserDirectory() / "vendor" / "premake" / "bin";

#ifdef ENGINE_PLATFORM_WINDOWS
			return basePath / "premake5.exe";
#elif ENGINE_PLATFORM_LINUX;
			return basePath / "premake5";
#endif
		}
	}

	ProjectManagerLayer::ProjectManagerLayer()
	{

	}

	void ProjectManagerLayer::OnAttach()
	{
		m_Config = ProjectManagerConfig::Load();

		if(m_Config->m_EditorPath.empty())
		{
			std::filesystem::path filepath = FileDialogs::OpenFile("Editor (Editor.exe)\0Editor.exe\0");
			if (!std::filesystem::exists(filepath))
			{
				ENGINE_CORE_ERROR("Couldn't find Editor Binaries");
				Application::Get().Close();
			}

			m_Config->m_EditorPath = filepath;
			m_Config->Save();
		}
	}

	void ProjectManagerLayer::OnDetach()
	{
		m_Config->Save();
	}
	void ProjectManagerLayer::OnUpdate(Timestep ts)
	{
		RenderCommand::BeginFrame(0);

		RenderCommand::EndFrame();
	}

	void ProjectManagerLayer::OnImGuiRender()
	{
		static bool dockingEnabled = false;
		static bool dockspaceOpen = false;
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

		ImGui::Begin("Projects");


		const int columns = 8;
		const int height = 160; // 32.0f (text size) + 128.0f (icon size)

		ImGui::Columns(columns);

		int index = 0;
		for (auto& project : m_Config->m_RecentProjects)
		{
			if (index % columns == 0) ImGui::Separator();
			if (ImGui::Button(project.filename().string().c_str(), ImVec2(ImGui::GetIO().DisplaySize.x / columns, height)))
			{
				OpenProject(project);
			}
			ImGui::NextColumn();
			index++;
		}

		ImGui::Columns(1);

		ImGui::Separator();
		
		if(ImGui::Button("Create New Project"))
		{
			NewProject();
		}
		ImGui::SameLine();
		if (ImGui::Button("Open Project..."))
		{
			OpenProject();
		}
		
		ImGui::End();

		ImGui::End();
	}

	void ProjectManagerLayer::OnEvent(Event& event)
	{

	}

	void ProjectManagerLayer::NewProject()
	{
		std::filesystem::path filepath = FileDialogs::SaveFile("Engine Project (*.eproj)\0*.eproj\0");
		if (filepath.empty())
			return;

		filepath.replace_extension(".eproj");

		Ref<Project> newProject = Project::New();

		std::filesystem::path projectDir = filepath.parent_path();

		std::filesystem::create_directories(projectDir / "Assets/Scenes");
		std::filesystem::create_directories(projectDir / "Assets/Textures");
		std::filesystem::create_directories(projectDir / "Scripts/Binaries");
		std::filesystem::create_directories(projectDir / "Scripts/Sources");

		std::string filename = filepath.filename().replace_extension().string();

		ProjectConfig& projectConfig = newProject->GetConfig();
		projectConfig.Name = filename;
		projectConfig.AssetDirectory = "Assets";
		projectConfig.AssetRegistryPath = "AssetRegistry.enr";
		projectConfig.ScriptModulePath = std::format("Scripts/Binaries/net9.0/{}.dll", filename);
		projectConfig.StartScene = 0;
		
		if (!HasPremakeInstalled())
			InstallPremake();

		std::ofstream premakeFileStream(projectDir / "Scripts/premake5.lua", std::ios::out);

		std::string premakeFileContent;
		std::regex_replace(std::back_inserter(premakeFileContent), premakeProjectFile.begin(), premakeProjectFile.end(),
			std::regex("ProjectName"), filename);

		premakeFileStream << premakeFileContent;

		premakeFileStream.close();

		std::ofstream assetRegistryFileStream(projectDir / "Assets/AssetRegistry.enr", std::ios::out);
		assetRegistryFileStream << "AssetRegistry:";
		assetRegistryFileStream.close();

		std::filesystem::path currentPath = Filesystem::GetCurrentDir();
		Filesystem::ChangeDirectory(projectDir / "Scripts");

		std::string premakeCommandLine = std::format("{} vs2022", Utils::ResolvePremakePath().string()).c_str();
		std::string msbuildCommandLine = std::format("dotnet build {}", std::filesystem::path(projectDir / "Scripts" / filename).replace_extension(".sln").string()).c_str();

		Process::StartProcess(premakeCommandLine.c_str());
		Process::StartProcess(msbuildCommandLine.c_str());
		Filesystem::ChangeDirectory(currentPath);

		Project::SaveActive(filepath);

		OpenProject(filepath);
	}

	bool ProjectManagerLayer::OpenProject()
	{
		std::filesystem::path filepath = FileDialogs::OpenFile("Engine Project (*.eproj)\0*.eproj\0");
		if (filepath.empty())
			return false;

		return OpenProject(filepath);
	}

	bool ProjectManagerLayer::OpenProject(const std::filesystem::path& path)
	{
		bool containsProject = false;
		for (const auto& project : m_Config->m_RecentProjects)
		{
			if (project == path)
			{
				containsProject = true;
				break;
			}
		}

		if (!containsProject)
		{
			m_Config->m_RecentProjects.push_back(path);
			m_Config->Save();
		}
		
		std::string commandLine = std::format("{} {}", m_Config->m_EditorPath.string().c_str(), path.string().c_str());

		Process::StartProcess(commandLine.c_str(), m_Config->m_EditorPath.parent_path().string().c_str(), false);

		Application::Get().Close();

		return true;
	}

	bool ProjectManagerLayer::HasPremakeInstalled()
	{
		return std::filesystem::exists(Utils::ResolvePremakePath());
	}

	bool ProjectManagerLayer::InstallPremake()
	{
		const std::string premakeVersion = "5.0.0-beta8";
		const std::string url = std::format("https://github.com/premake/premake-core/releases/download/v{0}/premake-{0}-windows.zip", premakeVersion);
		const std::filesystem::path premakeInstallDir = Utils::ResolvePremakePath().parent_path();
		const std::filesystem::path premakeZipFile = premakeInstallDir / "premake.zip";

		const std::string licenseUrl = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt";
		const std::filesystem::path licenseFile = premakeInstallDir / "LICENSE";

		Download::DownloadFile(url, premakeZipFile);
		Download::DownloadFile(licenseUrl, licenseFile);
		Download::UnzipFile(premakeZipFile, premakeInstallDir);		

		return true;
	}

}