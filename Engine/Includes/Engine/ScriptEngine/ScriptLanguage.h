#pragma once
#include "Engine/Core/Timestep.h"

namespace Engine
{
	class ScriptLanguage
	{
	public:
		virtual ~ScriptLanguage() = default;
		virtual StringName GetName() const = 0;
		virtual std::vector<StringName> GetExtensions() const = 0;
		
		virtual bool Init() = 0;
		virtual void Shutdown() = 0;
		virtual SharedPtr<Script> LoadScript(const std::filesystem::path& path) = 0;
		virtual SharedPtr<Script> GetScriptByClassName(const StringName& className) const = 0;

		virtual void ReloadScripts() = 0;
		
		virtual void OnRuntimeStart() = 0;
		virtual void OnRuntimeStop() = 0;

		virtual void Frame(Timestep ts) = 0;
	};
}