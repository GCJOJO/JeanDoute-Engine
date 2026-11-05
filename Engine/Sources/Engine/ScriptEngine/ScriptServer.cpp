#include "enginepch.h"
#include "Engine/ScriptEngine/ScriptServer.h"

namespace Engine
{

	ScriptServer& ScriptServer::Get()
	{

	}

	void ScriptServer::RegisterLanguage(Scope<ScriptLanguage> language)
	{

	}

	ScriptLanguage* ScriptServer::GetLanguage(const StringName& name) const
	{

	}

	ScriptLanguage* ScriptServer::GetLanguageForExtension(std::string_view extension) const
	{

	}

	bool ScriptServer::InitAll()
	{

	}

	void ScriptServer::ShutdownAll()
	{

	}

	Ref<Script> ScriptServer::LoadScript(const std::filesystem::path& path)
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

	}

	void ScriptServer::RuntimeStop()
	{

	}

	void ScriptServer::Frame(Timestep ts)
	{

	}

	void ScriptServer::ReloadAll(bool keepState /*= true*/)
	{

	}

}