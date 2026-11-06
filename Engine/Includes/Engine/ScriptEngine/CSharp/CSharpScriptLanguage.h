#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Types/StringName.h"
#include "Engine/Types/Resources/Script.h"
#include "Engine/ScriptEngine/ScriptLanguage.h"

#include <FileWatch.h>

#include <Coral/Assembly.hpp>
#include <Coral/HostInstance.hpp>

#include <filesystem>

namespace Engine
{
	class CSharpScriptLanguage : public ScriptLanguage
	{
	public:
	
		StringName GetName() const override { return "C#"; }
		std::vector<StringName> GetExtensions() const override { return { ".cs" }; }

		bool Init() override;

		Ref<Script> LoadScript(const std::filesystem::path& path) override;
		Ref<Script> GetScriptByClassName(const StringName& className) const override;
		void ReloadScripts() override;


		void OnRuntimeStart() override;
		void OnRuntimeStop() override;
		void Frame(Timestep ts) override;

	private:
		bool LoadCoreAssembly(const std::filesystem::path& path);
		bool LoadAppAssembly(const std::filesystem::path& path);

		void DiscoverScripts();
		void RegisterInternalCalls();

		void OnAssemblyFileChanged(const std::string& path);
		void OnManagedException(std::string_view message);

	private:
		Coral::HostInstance m_Host;
		Coral::AssemblyLoadContext m_LoadContext;
		Coral::ManagedAssembly* m_CoreAssembly = nullptr;
		Coral::ManagedAssembly* m_AppAssembly = nullptr;

		std::filesystem::path m_AppAssemblyPath;
		Scope<filewatch::FileWatch<std::string>> m_AssemblyWatcher;
		std::atomic<bool> m_ReloadPending{ false };
	};
}