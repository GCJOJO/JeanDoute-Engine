#pragma once
#include "Engine/Scene/Node.h"
#include "Engine/Types/Resources/Script.h"
#include "Engine/ScriptEngine/CSharp/CSharpScriptLanguage.h"

#include <Coral/Type.hpp>

namespace Engine
{
	class CSharpScript : public Script 
	{
	public:
		static Ref<CSharpScript> Create(CSharpScriptLanguage* language, Coral::Type& type);

		CSharpScript(CSharpScriptLanguage* language, Coral::Type& type);

		template <typename... TArgs>
		Coral::ManagedObject Instantiate(TArgs&... args)
		{
			m_Type->CreateInstance(std::forward<TArgs>(args)...);
		}

		const Coral::Type& GetCoralType() const { return *m_Type; }
		bool HasOnCreate() const { return m_HasOnCreate; }
		bool HasOnUpdate() const { return m_HasOnUpdate; }


		ScriptLanguage* GetLanguage() const override { return m_Language; }

		StringName GetBaseClassName() const override { return m_BaseClass; }
		bool CanInstantiate() const override;

		Scope<ScriptInstance> CreateInstance(Node* owner) override 
		{
			return CreateScope<ScriptInstance>(Ref<CSharpScript>(this), owner->GetId());
		}

		const ScriptMembers& GetMembers() const override { return m_Members; }
		bool HasMethod(const StringName& method) override;

	private:
		StringName m_ClassName;
		Coral::Type* m_Type = nullptr;
		StringName m_BaseClass;
		ScriptMembers m_Members;

		CSharpScriptLanguage* m_Language = nullptr;

		bool m_HasOnCreate = false;
		bool m_HasOnUpdate = false;
	};
}