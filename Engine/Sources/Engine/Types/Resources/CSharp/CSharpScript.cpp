#include "enginepch.h"
#include "Engine/Types/Resources/CSharp/CSharpScript.h"

namespace Engine
{
	Ref<CSharpScript> CSharpScript::Create(CSharpScriptLanguage* language, Coral::Type& type)
	{
		return CreateRef<CSharpScript>(language, type);
	}

	CSharpScript::CSharpScript(CSharpScriptLanguage* language, Coral::Type& type)
		: m_Language(language), m_Type(&type)
	{

	}

	bool CSharpScript::CanInstantiate() const
	{

	}

	Scope<ScriptInstance> CSharpScript::CreateInstance(Node* owner)
	{

	}

	bool CSharpScript::HasMethod(const StringName& method)
	{

	}
}