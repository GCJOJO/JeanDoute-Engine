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
		void Shutdown();

		SharedPtr<Script> LoadScript(const std::filesystem::path& path) override;
		SharedPtr<Script> GetScriptByClassName(const StringName& className) const override;
		void ReloadScripts() override;


		void OnRuntimeStart() override;
		void OnRuntimeStop() override;
		void Frame(Timestep ts) override;

	private:
		bool InitCoral(const std::filesystem::path& coralDir);

		void DiscoverScripts();
		void RegisterInternalCalls();

		void ReloadAssembly();
		void OnAssemblyFileChanged(const std::string& path);

	private:
		bool m_Initialized = false;
	};
}