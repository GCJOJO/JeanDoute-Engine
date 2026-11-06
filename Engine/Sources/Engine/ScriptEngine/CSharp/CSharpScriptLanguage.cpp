#include "enginepch.h"
#include "Engine/ScriptEngine/CSharp/CSharpScriptLanguage.h"
#include "Engine/Project/Project.h"
#include "Engine/Utils/PlatformUtils.h"
#include "Engine/Core/Application.h"
#include "Engine/ScriptEngine/ScriptServer.h"

namespace Engine
{
	static std::filesystem::path s_CoreAssemblyPath = Filesystem::GetExecutableDir() / "CSharp-Core.dll";

	static void OnMessage(std::string_view InMessage, Coral::MessageLevel InLevel)
	{
		switch (InLevel)
		{
		case Coral::MessageLevel::Trace:
			ENGINE_CORE_TRACE("[Script] : {}", InMessage);
			break;
		case Coral::MessageLevel::Info:
			ENGINE_CORE_INFO("[Script] : {}", InMessage);
			break;
		case Coral::MessageLevel::Warning:
			ENGINE_CORE_WARN("[Script] : {}", InMessage);
			break;
		case Coral::MessageLevel::Error:
			ENGINE_CORE_ERROR("[Script] : {}", InMessage);
			break;
		case Coral::MessageLevel::All:
			ENGINE_CORE_INFO("[Script] : {}", InMessage);
			break;
		default:
			ENGINE_CORE_INFO("[Script] : {}", InMessage);
			break;
		}
	}

	static void OnManagedException(std::string_view message)
	{
		ENGINE_CORE_ERROR("Unhandled native exception : {}", message);
	}

	struct CSharpData
	{
		Coral::HostInstance Host;
		Coral::HostSettings HostSettings;
		Coral::AssemblyLoadContext LoadContext;
		Coral::ManagedAssembly* CoreAssembly = nullptr;
		Coral::ManagedAssembly* AppAssembly = nullptr;

		std::filesystem::path AppAssemblyPath;
		Scope<filewatch::FileWatch<std::string>> AssemblyWatcher;

		std::atomic_bool AssemblyReloadPending{ false };
	};

	static CSharpData* s_CSharpData = new CSharpData();

	static void OnAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
	{
		if (!s_CSharpData->AssemblyReloadPending && change_type == filewatch::Event::modified)
		{
			s_CSharpData->AssemblyReloadPending = true;

			Application::Get().SubmitToMainThread([]()
			{
				s_CSharpData->AssemblyWatcher.reset();
				ScriptServer::Get().RequestReload();
			});
		}
	}

	static Coral::ManagedAssembly& LoadAssembly(const std::filesystem::path& assemblyPath)
	{
		auto& assembly = s_CSharpData->LoadContext.LoadAssembly(assemblyPath.string());

		s_CSharpData->AssemblyWatcher = CreateScope<filewatch::FileWatch<std::string>>(
			assemblyPath.string(), OnAppAssemblyFileSystemEvent);
		return assembly;
	}

	void CSharpScriptLanguage::ReloadAssembly()
	{
		Shutdown();
		Init();
	}

	bool CSharpScriptLanguage::Init()
	{
		s_CSharpData = new CSharpData();

		std::filesystem::path engineAssemblyDir = std::filesystem::path(s_CoreAssemblyPath).parent_path();

		if (!InitCoral(engineAssemblyDir))
			return false;

		s_CSharpData->CoreAssembly = &LoadAssembly(s_CoreAssemblyPath);
		if (s_CSharpData->CoreAssembly->GetLoadStatus() != Coral::AssemblyLoadStatus::Success)
		{
			ENGINE_CORE_ERROR("[Scripting Engine] Couldn't load Coral Assembly");
			return false;
		}

		s_CSharpData->NodeClass = ScriptClass("Engine.Entity", true);
		ScriptGlue::RegisterFunctions(*s_CSharpData->EngineAssembly);
		ScriptGlue::RegisterNodes();

		auto scriptModulePath = Project::GetActiveProjectDirectory() / Project::GetActive()->GetConfig().ScriptModulePath;
		s_CSharpData->AppAssembly = &LoadAssembly(scriptModulePath);
		if (s_CSharpData->AppAssembly->GetLoadStatus() != Coral::AssemblyLoadStatus::Success)
		{
			ENGINE_CORE_ERROR("[Scripting Engine] Couldn't load App Assembly");
			return false;
		}

		LoadAssemblyClasses();
		return true;
	}

	void CSharpScriptLanguage::Shutdown()
	{
		if (!m_Initialized)
			return;
	}

	SharedPtr<Script> CSharpScriptLanguage::LoadScript(const std::filesystem::path& path)
	{

	}

	SharedPtr<Script> CSharpScriptLanguage::GetScriptByClassName(const StringName& className) const
	{

	}

	void CSharpScriptLanguage::ReloadScripts()
	{

	}

	void CSharpScriptLanguage::OnRuntimeStart()
	{

	}

	void CSharpScriptLanguage::OnRuntimeStop()
	{

	}

	void CSharpScriptLanguage::Frame(Timestep ts)
	{

	}

	bool CSharpScriptLanguage::InitCoral(const std::filesystem::path& coralDir)
	{
		s_CSharpData->HostSettings.CoralDirectory = coralDir.string();
		s_CSharpData->HostSettings.MessageCallback = OnMessage;
		s_CSharpData->HostSettings.ExceptionCallback = OnManagedException;

		auto initStatus = s_CSharpData->Host.Initialize(s_CSharpData->HostSettings);
		if (initStatus != Coral::CoralInitStatus::Success)
		{
			ENGINE_ASSERT(false, "Coral not initalized");
			return false;
		}

		s_CSharpData->LoadContext = s_CSharpData->Host.CreateAssemblyLoadContext("RuntimeContext");
		return true;
	}

	void CSharpScriptLanguage::DiscoverScripts()
	{

	}

	void CSharpScriptLanguage::RegisterInternalCalls()
	{

	}

	void CSharpScriptLanguage::OnAssemblyFileChanged(const std::string& path)
	{

	}
}