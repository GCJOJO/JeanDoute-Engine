#pragma once
#include "ScriptLanguage.h"
#include "Engine/Core/Core.h"
#include "Engine/Types/StringName.h"
#include "Engine/Types/EngineSingleton.h"
#include "Engine/Types/Resources/Script.h"


#include <vector>

namespace Engine
{
	class ScriptServer : public EngineSingleton
	{
	public:
		static ScriptServer& Get() { return m_Instance; }
		static void Init();

		void RegisterLanguage(Scope<ScriptLanguage> language);
		ScriptLanguage* GetLanguage(const StringName& name) const;
		ScriptLanguage* GetLanguageForExtension(std::string_view extension) const;
		
		const std::vector<Scope<ScriptLanguage>>& GetLanguages() const { return m_Languages; }

		bool InitAll();
		void ShutdownAll();

		SharedPtr<Script> LoadScript(const std::filesystem::path& path);
		
		void RegisterInstance(ScriptInstance* instance);
		void UnregisterInstance(ScriptInstance* instance);

		void RuntimeStart();
		void RuntimeStop();
		void Frame(Timestep ts);

		void ReloadAll(bool keepState = true);
		void RequestReload();

	private:
		static ScriptServer m_Instance;
		std::vector<Scope<ScriptLanguage>> m_Languages{ };
		std::unordered_map<StringName, ScriptLanguage*> m_ByExtensions{ };
		std::atomic_bool m_ReloadRequested = { false };
	};
}