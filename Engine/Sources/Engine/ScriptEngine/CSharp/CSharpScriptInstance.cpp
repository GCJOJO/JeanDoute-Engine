#include "enginepch.h"
#include "Engine/ScriptEngine/CSharp/CSharpScriptInstance.h"

namespace Engine
{

	CSharpScriptInstance::CSharpScriptInstance(SharedPtr<CSharpScript> script, Node* owner)
		: m_Script(script), m_Owner(owner)
	{
		m_Object = m_Script->Instantiate(owner->GetId());
	}

	bool CSharpScriptInstance::HasMethod(const StringName& method) const
	{
		return m_Script->HasMethod(method);
	}

	bool CSharpScriptInstance::Call(const StringName& method, Variant* args, std::size_t argCount, Variant& result)
	{

	}

	bool CSharpScriptInstance::Get(const StringName& property, Variant& out) const
	{

	}

	bool CSharpScriptInstance::Set(const StringName& property, const Variant& value)
	{

	}

	bool CSharpScriptInstance::GetPropertyList(std::vector<ScriptPropertyInfo>& out) const
	{

	}

	void CSharpScriptInstance::Notification(int what)
	{

	}

	ScriptState CSharpScriptInstance::SaveState() const
	{

	}

	void CSharpScriptInstance::RestoreState(const ScriptState& state)
	{

	}

}