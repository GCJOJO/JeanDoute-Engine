#include "enginepch.h"
#include "Engine/ScriptEngine/ScriptServer.h"

namespace Engine
{

	void ScriptServer::Init()
	{
		m_Instance{ };
	}

	void ScriptServer::RegisterLanguage(Scope<ScriptLanguage> language)
	{
		m_Languages.push_back(language);
		for (const StringName& extension : language->GetExtensions())
			m_ByExtensions.emplace(extension, language.get());
		
	}

	ScriptLanguage* ScriptServer::GetLanguage(const StringName& name) const
	{
		for (const Scope<ScriptLanguage>& language : m_Languages)
		{
			if (language->GetName() == name)
				return language.get();
		}

		return nullptr;
	}

	ScriptLanguage* ScriptServer::GetLanguageForExtension(std::string_view extension) const
	{
		auto it = m_ByExtensions.find(extension);
		return it != m_ByExtensions.end() ? it->second : nullptr;
	}

	bool ScriptServer::InitAll()
	{
		for (const Scope<ScriptLanguage>& language : m_Languages)
		{
			if (!language->Init())
			{
				ENGINE_CORE_ERROR("Cannot init ScriptLanguage : {}", language->GetName());
				// TODO Mark Language as Unusable
			}
		}
	}

	void ScriptServer::ShutdownAll()
	{
		for (const Scope<ScriptLanguage>& language : m_Languages)
			language->Shutdown();
	}

	SharedPtr<Script> ScriptServer::LoadScript(const std::filesystem::path& path)
	{

	}

	void ScriptServer::RegisterInstance(ScriptInstance* instance)
	{

	}

	void ScriptServer::UnregisterInstance(ScriptInstance* instance)
	{

	}

	void ScriptServer::RuntimeStart()
	{
		for (const Scope<ScriptLanguage>& language : m_Languages)
			language->OnRuntimeStart();
	}

	void ScriptServer::RuntimeStop()
	{
		for (const Scope<ScriptLanguage>& language : m_Languages)
			language->OnRuntimeStop();
	}

	void ScriptServer::Frame(Timestep ts)
	{
		for (const Scope<ScriptLanguage>& language : m_Languages)
			language->Frame(ts);
	}

	void ScriptServer::ReloadAll(bool keepState /*= true*/)
	{

	}

	void ScriptServer::RequestReload()
	{
		m_ReloadRequested.store(true);
	}

}