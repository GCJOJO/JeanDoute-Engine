#pragma once
#include "Engine/ScriptEngine/ScriptInstance.h"
#include "Engine/Types/Resources/CSharp/CSharpScript.h"

namespace Engine
{
	class CSharpScriptInstance : public ScriptInstance
	{
	public:
		CSharpScriptInstance(SharedPtr<CSharpScript> script, Node* owner);

		SharedPtr<Script> GetScript() const override { return m_Script; }
		Node* GetOwner() const override { return m_Owner; }


		bool HasMethod(const StringName& method) const override;
		bool Call(const StringName& method, Variant* args, std::size_t argCount, Variant& result) override;
		bool Get(const StringName& property, Variant& out) const override;
		bool Set(const StringName& property, const Variant& value) override;
		bool GetPropertyList(std::vector<ScriptPropertyInfo>& out) const override;

		void Notification(int what) override;

		ScriptState SaveState() const override;
		void RestoreState(const ScriptState& state) override;

	private:
		SharedPtr<CSharpScript> m_Script;
		Node* m_Owner = nullptr;
		Coral::ManagedObject m_Object;
	};
}