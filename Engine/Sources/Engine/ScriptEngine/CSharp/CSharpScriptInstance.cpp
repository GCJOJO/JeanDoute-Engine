#include "enginepch.h"
#include "Engine/ScriptEngine/CSharp/CSharpScriptInstance.h"

namespace Engine
{

	CSharpScriptInstace::CSharpScriptInstace(Ref<CSharpScript> script, Node* owner)
		: m_Script(script), m_Owner(owner)
	{
		m_Object = m_Script->Instantiate(owner);
	}

	bool CSharpScriptInstace::HasMethod(const StringName& method) const
	{
		return m_Script->HasMethod(method);
	}

	bool CSharpScriptInstace::Call(const StringName& method, Variant* args, std::size_t argCount, Variant& result)
	{

	}

	bool CSharpScriptInstace::Get(const StringName& property, Variant& out) const
	{

	}

	bool CSharpScriptInstace::Set(const StringName& property, const Variant& value)
	{

	}

	bool CSharpScriptInstace::GetPropertyList(std::vector<ScriptPropertyInfo>& out) const
	{

	}

	void CSharpScriptInstace::Notification(int what)
	{

	}

	ScriptState CSharpScriptInstace::SaveState() const
	{

	}

	void CSharpScriptInstace::RestoreState(const ScriptState& state)
	{

	}

}